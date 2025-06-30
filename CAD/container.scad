use <StopLight.scad>

MATERIAL_THICKNESS = 1;
TOTAL_HEIGHT = 139;
MARGIN = .7;
BEND_LENGTH = (MARGIN + (MATERIAL_THICKNESS/2)) * PI/2;
echo("Bend length:", BEND_LENGTH);

/* [Global resolution] */
// Minimal facet size
$fs = 0.1;  // [0.1:0.01:10]
// Minimal angle size
$fa = 1;    // [1:0.1:90]

rotate([-90,0,0])translate([0,-25,95-(TOTAL_HEIGHT/2)])union() {
    housing(shades=true, lights=3);
    housing_back(3);
    pole(80);
    base_door();
}

//#translate([0,0,25])cube([50,TOTAL_HEIGHT,50],center=true);

translate([0,0,-(MATERIAL_THICKNESS/2)-MARGIN])cube([50,TOTAL_HEIGHT+(4*MARGIN)+(2*MATERIAL_THICKNESS),MATERIAL_THICKNESS],center=true);

translate([25,0,0])rotate([-90,0,0])rotate_extrude(angle = 90)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,TOTAL_HEIGHT+2*MARGIN+MATERIAL_THICKNESS],center=true);
translate([-25,0,0])rotate([-90,0,180])rotate_extrude(angle = 90)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,TOTAL_HEIGHT+2*MARGIN+MATERIAL_THICKNESS],center=true);

translate([(MATERIAL_THICKNESS/2)+MARGIN+25,0,25])cube([MATERIAL_THICKNESS,TOTAL_HEIGHT+2*MARGIN+MATERIAL_THICKNESS,50],center=true);
translate([-25-(MATERIAL_THICKNESS/2)-MARGIN,0,25])cube([MATERIAL_THICKNESS,TOTAL_HEIGHT+2*MARGIN+MATERIAL_THICKNESS,50],center=true);

translate([0,(TOTAL_HEIGHT/2)+(2*MARGIN)+MATERIAL_THICKNESS,0])rotate([-90,0,90])rotate_extrude(angle = 90)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,50],center=true);
translate([0,-(TOTAL_HEIGHT/2)-(2*MARGIN)-MATERIAL_THICKNESS,0])rotate([-90,0,-90])rotate_extrude(angle = 90)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,50],center=true);

translate([0,(TOTAL_HEIGHT/2)+MARGIN+(MATERIAL_THICKNESS/2),24])cube([50,MATERIAL_THICKNESS,48],center=true);
translate([0,-(TOTAL_HEIGHT/2)-MARGIN-(MATERIAL_THICKNESS/2),24])cube([50,MATERIAL_THICKNESS,48],center=true);
translate([0,(TOTAL_HEIGHT/2)+(MARGIN*3)+(1.5*MATERIAL_THICKNESS),24])cube([50,MATERIAL_THICKNESS,48],center=true);
translate([0,-(TOTAL_HEIGHT/2)-(MARGIN*3)-(1.5*MATERIAL_THICKNESS),24])cube([50,MATERIAL_THICKNESS,48],center=true);

translate([0,(TOTAL_HEIGHT/2)+(2*MARGIN)+MATERIAL_THICKNESS,48])rotate([90,0,90])rotate_extrude(angle = 180)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,50],center=true);
translate([0,-(TOTAL_HEIGHT/2)-(2*MARGIN)-MATERIAL_THICKNESS,48])rotate([90,0,-90])rotate_extrude(angle = 180)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,50],center=true);

translate([13,(TOTAL_HEIGHT/2)+(2*MARGIN)+(MATERIAL_THICKNESS),24])cube([24,MATERIAL_THICKNESS,48],center=true);
translate([-13,(TOTAL_HEIGHT/2)+(2*MARGIN)+(MATERIAL_THICKNESS),24])cube([24,MATERIAL_THICKNESS,48],center=true);
translate([13,-(TOTAL_HEIGHT/2)-(2*MARGIN)-(MATERIAL_THICKNESS),24])cube([24,MATERIAL_THICKNESS,48],center=true);
translate([-13,-(TOTAL_HEIGHT/2)-(2*MARGIN)-(MATERIAL_THICKNESS),24])cube([24,MATERIAL_THICKNESS,48],center=true);

translate([25,(TOTAL_HEIGHT/2)+(MARGIN)+(MATERIAL_THICKNESS/2),24])rotate([0,0,0])rotate_extrude(angle = 90)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,48],center=true);
translate([-25,(TOTAL_HEIGHT/2)+(MARGIN)+(MATERIAL_THICKNESS/2),24])rotate([0,0,90])rotate_extrude(angle = 90)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,48],center=true);
translate([25,-(TOTAL_HEIGHT/2)-(MARGIN)-(MATERIAL_THICKNESS/2),24])rotate([0,0,-90])rotate_extrude(angle = 90)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,48],center=true);
translate([-25,-(TOTAL_HEIGHT/2)-(MARGIN)-(MATERIAL_THICKNESS/2),24])rotate([0,0,180])rotate_extrude(angle = 90)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,48],center=true);

translate([-25,0,50])rotate([-90,-90,180])rotate_extrude(angle = 90)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,TOTAL_HEIGHT+2*MARGIN+MATERIAL_THICKNESS],center=true);
translate([25+(MARGIN)+(MATERIAL_THICKNESS/2),0,50])rotate([-90,-90,0])rotate_extrude(angle = 90)translate([MARGIN+(MATERIAL_THICKNESS/2),0])square([MATERIAL_THICKNESS,TOTAL_HEIGHT+2*MARGIN+MATERIAL_THICKNESS],center=true);

translate([(MARGIN/2)+(MATERIAL_THICKNESS/4),0,50+(MATERIAL_THICKNESS/2)+MARGIN])cube([50+MARGIN+(MATERIAL_THICKNESS/2),TOTAL_HEIGHT+2*MARGIN+MATERIAL_THICKNESS,MATERIAL_THICKNESS],center=true);
intersection() {
    translate([(MATERIAL_THICKNESS)+(MARGIN*2)+25,0,25])cube([MATERIAL_THICKNESS,TOTAL_HEIGHT+2*MARGIN+MATERIAL_THICKNESS,50],center=true);
    hull(){
        translate([(MATERIAL_THICKNESS)+(MARGIN*2)+25,(TOTAL_HEIGHT/2)-20+MARGIN+(MATERIAL_THICKNESS/2),50])rotate([0,90,0])cylinder(r=20,h=2*MATERIAL_THICKNESS,center=true);
        translate([(MATERIAL_THICKNESS)+(MARGIN*2)+25,-(TOTAL_HEIGHT/2)+20-MARGIN-(MATERIAL_THICKNESS/2),50])rotate([0,90,0])cylinder(r=20,h=2*MATERIAL_THICKNESS,center=true);
        translate([(MATERIAL_THICKNESS)+(MARGIN*2)+25,0,20])rotate([0,90,0])cylinder(r=20,h=2*MATERIAL_THICKNESS,center=true);
    }
}