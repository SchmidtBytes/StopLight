#ifndef STOP_LIGHT_WIFI_MESSAGE_H
#define STOP_LIGHT_WIFI_MESSAGE_H

#include "WString.h"
#include <NetworkClient.h>
#include "state_machine.h"
#include "wifi_favicons.h"

// create the html code for a button
String create_button(String name, String link, String tooltip="")
{
    if(tooltip.equals(""))
    {
        return String("<button class=\"button1\" onclick=\"document.location='" + link + "'\">" + name + "</button>");
    }
    return String("<button class=\"button1\" onclick=\"document.location='" + link + "'\" title=\"" + tooltip + "\">" + name + "</button>");
}

// Convert the current wifi client state to a String
String get_wifi_state() {
  switch (WiFi.status())
  {
    case WL_NO_SHIELD:
      return "No Shield";
    case WL_STOPPED:
      return "Stopped";
    case WL_IDLE_STATUS:
      return "Idle";
    case WL_NO_SSID_AVAIL:
      return "No SSID available";
    case WL_SCAN_COMPLETED:
      return "Scan completed";
    case WL_CONNECTED:
      return "Connected";
    case WL_CONNECT_FAILED:
      return "Connection failed";
    case WL_CONNECTION_LOST:
      return "Connection lost";
    case WL_DISCONNECTED:
      return "Disconnected";
    default:
      return "Unknown";
  }
}

// Send the HTML response page
void send_html_response(NetworkClient &client, String requested_page, uint8_t message  = MSG_OFF)
{
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // the content of the HTTP response follows the header:
  client.print("<html>");
  client.print("<header>");
  client.print("<title>Stop Light</title>");
  client.print("<link rel=\"icon\" type=\"image/x-icon\" href=\"");
  client.print(get_favicon(active_state));
  client.print("\">");
  client.print("</header>");

  client.print("<style>");
  if(message != MSG_OFF)
  {
    client.print(".msgbox {");
    if (message == MSG_STATE_UPDATE || message == MSG_SLEEP_UPDATE)
    {
      client.print("background-color:LightGreen;");
    }
    else
    {
      client.print("background-color:DarkRed;color:White;");
    }
    client.print("width:414px;height:25px;line-height: 25px;}");
  }
  client.print(".button1 {width: 100px; height: 50px; font-size: 16px;}");
  client.print(".footer {position: fixed;right: 10;bottom: 10;width: 100%;text-align: right;}");
  client.print("</style>");

  client.print("<body>");
  client.print("<H1><p><center>Stop Light Control</center></H1><p>");

  // Message box
  if(message == MSG_OFF) {}
  else if(message == MSG_STATE_UPDATE)
  {
    client.print("<center><div class=\"msgbox\">Status set to " + STATE_STRING_MAP.at(active_state) + "</div><p></center>");
  }
  else if(message == MSG_SLEEP_UPDATE)
  {
    client.print("<center><div class=\"msgbox\">Sleep mode was " + String(requested_page.equals("e")?"enabled":"disabled") + "</div><p></center>");
  }
  else if(message == MSG_UNKNOWN_CMD)
  {
    client.print("<center><div class=\"msgbox\">Command " + requested_page + " unknown</div><p></center>");
  }
  else
  {
    client.print("<center><div class=\"msgbox\">Error type unknown: " + String(message) + "</div><p></center>");
  }

  // Table for the buttons
  client.print("<center><table><tr>");
  client.print("<td>" + create_button("Stop",          "f", "Red state indicating failure") + "</td>");
  client.print("<td>" + create_button("Init",          "i", "Run through the initialization animation and then turn off") + "</td>");
  client.print("<td>" + create_button("Wait",          "w", "Red and green flashing") + "</td>");
  client.print("<td>" + create_button("Cycle",         "c", "Run the stop light cycle as a loop") + "</td>");
  client.print("</tr><tr>");
  client.print("<td>" + create_button("Prepare",       "p", "Show the red and yellow phase") + "</td>");
  client.print("<td>" + create_button("Off",           "o", "Turn the stop light off") + "</td>");
  client.print("<td>" + create_button("Alternating",   "a", "Red and green showing together and alternating with yellow") + "</td>");
  client.print("</tr><tr>");
  client.print("<td>" + create_button("Go",            "s", "Green state indicating success") + "</td>");
  client.print("<td>" + create_button("Night mode",    "n", "Yellow light flashing on and off") + "</td>");
  client.print("<td>" + create_button("Rolling",       "r", "Animation rolling through the colors") + "</td>");
  client.print("<td>" + create_button("Enable sleep",  "e", "Enable the sleep mode feature. This will be saved over power cycles") + "</td>");
  client.print("</tr><tr>");
  client.print("<td>" + create_button("Hold",          "u", "Yellow state indicating unstable build") + "</td>");
  client.print("<td>" + create_button("Dimmed mode",   "b", "Yellow light dimming on and off, used for build indication") + "</td>");
  client.print("<td>" + create_button("Party",         "$", "Colors, brightness and durations selected at random") + "</td>");
  client.print("<td>" + create_button("Disable sleep", "d", "Disable the sleep mode feature. This will be saved over power cycles") + "</td>");
  client.print("</tr></table></center><p>");

  // State
  client.print("<center>state: ");
  client.print(STATE_STRING_MAP.at(active_state));
  client.print("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; sleep: ");
  client.print(sleep_active?"Enabled":"Disabled");
  client.print("</center><p>");

  // Reload
  client.print("<center>" + create_button("Reload", ".", "Reload to update the state") + "</center>");

  // Page footer
  client.print("<div class=\"footer\"><a href=\"/state\">state api</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"https://github.com/SchmidtBytes/StopLight\">github</a></div>");

  client.print("</body>");
  client.print("</html>");
  // The HTTP response ends with another blank line:
  client.println();
}

// Send the JSON response page
void send_json_status(NetworkClient &client)
{
  String state_str = STATE_STRING_MAP.at(active_state);
  state_str.toLowerCase();

  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println();

  // JSON response
  // Operation state
  client.print("{\"status\":\"");
  client.print(state_str);
  client.print("\",\"sleep_active\":");
  client.print(sleep_active?"true":"false");
  client.print(",\"sleep_time\":");
  if(sleep_active && active_state != STATE_OFF)
  {
    client.print(String(((sleep_timer * CYCLE_TIME) + 500) / 1000));
  }
  else
  {
    client.print("-1");
  }

  // WIFI state
  // AP state
  client.print(",\"wifi\":{\"ap\":{");
#ifdef WIFI_ENABLE_AP
  client.print("\"enabled\":true,\"ssid\":\"" + String(ssid_ap));
  client.print("\",\"password\":\"" + String(password_ap));
  client.print("\",\"ip\":\"" + String(ap_ip & 0xFF) + "." + String((ap_ip >> 8) & 0xFF) + "." + String((ap_ip >> 16) & 0xFF) + "." + String(ap_ip >> 24) + "\"");
#else
  client.print("\"enabled\":false,\"ssid\":\"-\",\"password\":\"-\",\"ip\":\"-\"");
#endif
  // client state
  client.print("},\"client\":{");
#ifdef WIFI_ENABLE_CLIENT
  client.print("\"enabled\":true,\"ssid\":\"" + String(ssid));
  client.print("\",\"state\":\"");
  client.print(get_wifi_state());
  IPAddress ip = WiFi.localIP();
  client.print("\",\"ip\":\"" + String(ip & 0xFF) + "." + String((ip >> 8) & 0xFF) + "." + String((ip >> 16) & 0xFF) + "." + String(ip >> 24) + "\"");
#else
  client.print("\"enabled\":false,\"ssid\":\"-\",\"state\":\"-\",\"ip\":\"-\"");
#endif
  client.print("}}");
  client.print("}");
  client.println();
}

#endif // STOP_LIGHT_WIFI_MESSAGE_H
