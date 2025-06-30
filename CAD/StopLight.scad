/* [Customization] */
// Ammount of LEDs for the stop light
NUM_LIGHTS       = 3; // [20]
// Add or remove the sunshades
SHADES           = true;
// The length of the stop light pole
POLE_LENGTH      = 80; // [2:0.01:200]
// Make the pole a separate object
INDEPENDENT_POLE = true;
// The inner diameter of the pole
POLE_DI          = 2; // [0:0.01:8]
// The controller used in the stop light
VARIANT             = "Arduino Nano"; // ["Arduino Nano", "ESP32-C6 Supermini"]
// The locking mechanism attaching the base door to the base
BASE_LOCK           = "Clip"; // ["Clip", "Screw"]

/* [Enable/Disable parts] */
// Show the LED housing
ENABLE_LED_HOUSING = true;
// Show the back of the LED housing
ENABLE_BACK_PANEL  = true;
// Show the pole
ENABLE_POLE        = true;
// Show the pole base
ENABLE_BASE        = true;
// Show the base door
ENABLE_BASE_DOOR   = true;

/* [Cosmetic options] */
// Show the LEDs
SHOW_LEDs        = false;
// Show the Arduino
SHOW_CONTROLLER  = false;
// Show a cross section of the stop light
CROSS_SECTION    = false;
// The angle to cut the cross section at
CROSS_SECTION_ANGLE = 0; // [-90:45:90]

/* [Global resolution] */
// Minimal facet size
$fs = 0.1;  // [0.1:0.01:10]
// Minimal angle size
$fa = 1;    // [1:0.1:90]


if(SHOW_LEDs) {
    if(NUM_LIGHTS > 1)
        translate([0,0,6+(15*(NUM_LIGHTS-1))])LED("#ff000080");
    if(NUM_LIGHTS > 2) {
        for(i=[0:NUM_LIGHTS-3]) {
            translate([0,0,6+(15*(i+1))])LED("#ffff0080");
        }
    }
    translate([0,0,6])LED("#00ff0080");
}

if(SHOW_CONTROLLER && VARIANT == "Arduino Nano") {
    translate([0,1.75,-POLE_LENGTH-8.5])arduino_nano();
}
if(SHOW_CONTROLLER && VARIANT == "ESP32-C6 Supermini") {
    translate([0,11.8,-POLE_LENGTH-8.5])ESP32_C3_SuperMini();
}

difference() {
    union() {
        if(ENABLE_LED_HOUSING) {
            housing(shades=SHADES, lights=NUM_LIGHTS);
        }
        if(ENABLE_BACK_PANEL) {
            housing_back(lights=NUM_LIGHTS);
        }
        pole(POLE_LENGTH);
        if(ENABLE_BASE_DOOR) {
            base_door();
        }
    }
    if(CROSS_SECTION) {
        rotate([0,0,CROSS_SECTION_ANGLE])translate([0,-40,-POLE_LENGTH-30])cube([50,80,40+(15*NUM_LIGHTS)+POLE_LENGTH]);
    }
}

module LED(col="") {
    color(col) union() {
        translate([0,0.5,0])rotate([90,0,0])cylinder(d=10,h=7);
        rotate([-90,0,0])cylinder(d=11,h=2);
        translate([0,-6.5,0])sphere(d=10);
    }
}

module housing_back(lights = 3) {
    box_height = 16 + ((lights-1)*15);
    color("#404040")translate([0,7.5,(box_height/2)-2])
    slanted_cube(16-2.2,1,box_height-2.2);
    color("#404040")translate([0,7.75,(box_height/2)-2])
    cube([16-2.2,0.5,box_height-2.2],center=true);
}

module housing(shades=false,lights = 3) {
    box_height = 16 + ((lights-1)*15);
    color("#202020")translate([0,0,(box_height/2)-2])difference() {
        // main body
        slanted_cube(16,16,box_height);

        for(i=[0:lights-1]) {
            // LEDs
            translate([0,0,((15*(lights-1))/2)-(15*i)])rotate([90,0,0])linear_extrude(height=8.01,scale=0.95)circle(d=10.6);
            translate([0,-0.20,((15*(lights-1))/2)-(15*i)])rotate([-90,0,0])cylinder(d=12.5,h=10);
        }

        // remove interior
        translate([0,7.8,0])cube([10,16,box_height-6],center=true);
        translate([0,14.8,0])cube([14,16,box_height-2],center=true);

        // pole
        translate([0,0,(-box_height/2)+2])cylinder(d=2,h=5,center=true);
        translate([0,0,(-box_height/2)-1])cylinder(d=6.2,h=6,center=true);
    }

    // shields
    if(shades) {
        for(j=[0:lights-1]) {
            color("#000000")translate([0,0,(box_height/2)-2])difference () {
                translate([0,-7,((15*(lights-1))/2)-(15*j)+0.1])rotate([90,0,0])cylinder(d=14,h=6);
                translate([0,-6.5,((15*(lights-1))/2)-(15*j)-1.9])rotate([90,0,0])cylinder(d=14,h=7);
                translate([0,-10,((15*(lights-1))/2)-(15*j)])rotate([110,0,0])cylinder(d=17,h=7);
            }
        }
    }
}

module pole(length=70) {
    if(ENABLE_POLE) {
        color("gray")translate([0,0,-length/2])difference() {
            union() {
                cylinder(d=6,h=length,center=true);
                if(INDEPENDENT_POLE) {
                    translate([0,0,-2])cylinder(d=8,h=length,center=true);
                } else {
                    translate([0,0,-1])cylinder(d=8,h=length-2,center=true);
                }
            }
            cylinder(d=POLE_DI,h=length+10,center=true);
            translate([0,0,length/2])rotate([-90,0,0])cylinder(d=2,h=6);
        }
    }

    if(ENABLE_BASE){
        translate([0,0,-length])base();
    }
}

module base_door() {
    color("gray")translate([0,0,-15-POLE_LENGTH]) difference() {
        union() {
            difference() {
                union() {
                    translate([0,0,0.75])slanted_cube(46.6,46.6,1.5);
                    if(BASE_LOCK == "Clip") {
                        translate([-20,0,4])rotate([90,0,0])cylinder(d=3.6, h=20,center=true);
                    }
                    if(BASE_LOCK == "Screw") {
                        translate([-17,0,2.5])cylinder(d1=10.8,d2=8.8,h=2,center=true);
                    }
                }
                if(BASE_LOCK == "Clip") {
                    translate([-21.5,0,0.9])cube([15,22,2],center=true);
                    translate([-17,0,0.9])cube([4,21,10],center=true);
                }
                if(BASE_LOCK == "Screw") {
                    translate([-17,0,-2])cylinder(d=3.2,h=15,center=true);
                    translate([-17,0,.99])cylinder(d2=3.2,d1=6.4,h=2,center=true);
                }
            }
            if(BASE_LOCK == "Clip") {
                translate([-8,0,0]) difference() {
                    rotate([90,0,0])cylinder(d=26, h=20,center=true);
                    rotate([90,0,0])cylinder(d=22, h=21,center=true);
                    translate([0,0,-10])cube([50,50,20],center=true);
                    translate([21,0,8])cube([50,50,20],center=true);
                }
                translate([-13,0,5.5])cube([2,20,11],center=true);
            }
            translate([15.8,-10,5])rotate([90,0,0])linear_extrude(height=10)polygon([[0,-5],[5,1],[7,1],[7,-0.3],[5,-0.8],[5,-5]]);
            translate([15.8,20,5])rotate([90,0,0])linear_extrude(height=10)polygon([[0,-5],[5,1],[7,1],[7,-0.3],[5,-0.8],[5,-5]]);
            if(BASE_LOCK == "Clip") {
                translate([0,-18,0])cylinder(d=4,h=5.4);
            }
            if(BASE_LOCK == "Screw") {
                translate([-7,2,0])cylinder(d=4,h=5.95);
                translate([7,2,0])cylinder(d=4,h=5.95);
            }
        }
        if(BASE_LOCK == "Clip") {
            translate([-11.5,0,12])rotate([0,45,0])cube([3,22,3],center=true);
        }
    }
}

module base() {
    color("gray")translate([0,0,-7.5])difference() {
        slanted_cube(50,50,15);
        translate([0,0,3.8])linear_extrude(height=6,scale=0)circle(d=5);
        translate([0,0,-13.3])cube([47,47,15],center=true);
        translate([0,0,-5+2])cube([20,44,15],center=true);
        translate([16,0,-5+4])cube([10,44,15],center=true);
        translate([18,0,-5+4+2.5])cube([10,44,10],center=true);
        if(BASE_LOCK == "Clip") {
            translate([-16,0,-5+4])cube([10,44,15],center=true);
            translate([-20,0,-6.5+3])rotate([90,0,0])cylinder(d=4, h=22,center=true);
        }
        else if(BASE_LOCK == "Screw") {
            translate([-17,0,-2])cylinder(d=4.1,h=15,center=true);
            translate([-17,0,-4.81])cylinder(d1=11,d2=9,h=2,center=true);
        }
        cylinder(d=POLE_DI, h=16);
        if(INDEPENDENT_POLE) {
            translate([0,0,5.3])cylinder(d=8.3, h=16);
        }
        if(VARIANT == "Arduino Nano") {
            translate([0,1.75,-10.5+7.5+2])arduino_nano();
        }
        else if(VARIANT == "ESP32-C6 Supermini") {
            translate([0,11.8,-10.5+7.5+2])ESP32_C3_SuperMini();
        }
    }
    if(VARIANT == "Arduino Nano") {
        color("gray")difference() {
            translate([0,-19,-5])cube([10,10,8],center=true);
            translate([0,1.75,-10.5+2])arduino_nano();
        }
    }
    else if(VARIANT == "ESP32-C6 Supermini") {
        color("gray")difference() {
            union() {
                translate([-10,0,-9])cylinder(d=5, h=8);
                translate([10,0,-9])cylinder(d=5, h=8);
            }
            translate([0,11.8,-10.5+2])ESP32_C3_SuperMini();
            translate([13.5,0,-6])cube([6,6,10],center=true);
        }
    }
}

module ESP32_C3_SuperMini() {
    color("blue")translate([0,-.25,0])cube([18.3,23.2,1.1],center=true);
    color("#404040")translate([0,5.75,1.1/2])rotate([-90,0,0])hull() {
        translate([(8.9-3.1)/2,-3.1/2,0])cylinder(d=3.4, h=7.5);
        translate([-(8.9-3.1)/2,-3.1/2,0])cylinder(d=3.4, h=7.5);
    }
    color("#202020")translate([-5.8,10,2.1/2])cube([2.75,3,1],center=true);
}

module arduino_nano() {
    color("blue")translate([0,-.25,0])cube([18.8,44,1.8],center=true);
    color("blue")translate([4,20,-1.9])cube([4,3.5,2.1],center=true);
    color("gray")translate([0,19,2.8])cube([8,10.5,4.2],center=true);
}

module slanted_cube(x, y, z, edge=0.5) {
    intersection() {
        translate([0,0,-z/2])linear_extrude(height=z)polygon([[(x/2)-edge,y/2],[x/2,(y/2)-edge],[x/2,(-y/2)+edge],[(x/2)-edge,-y/2],[(-x/2)+edge,-y/2],[-x/2,(-y/2)+edge],[-x/2,(y/2)-edge],[(-x/2)+edge,y/2]]);
        rotate([90,0,0])translate([0,0,-y/2])linear_extrude(height=y)polygon([[(x/2)-edge,z/2],[x/2,(z/2)-edge],[x/2,(-z/2)+edge],[(x/2)-edge,-z/2],[(-x/2)+edge,-z/2],[-x/2,(-z/2)+edge],[-x/2,(z/2)-edge],[(-x/2)+edge,z/2]]);
        rotate([0,90,0])translate([0,0,-x/2])linear_extrude(height=x)polygon([[(z/2)-edge,y/2],[z/2,(y/2)-edge],[z/2,(-y/2)+edge],[(z/2)-edge,-y/2],[(-z/2)+edge,-y/2],[-z/2,(-y/2)+edge],[-z/2,(y/2)-edge],[(-z/2)+edge,y/2]]);
    }
}
