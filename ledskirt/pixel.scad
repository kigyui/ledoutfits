// A tray you can push those little Neopixel circut boards into
// see https://kigyui.com/ledskirt/

// This is a postive of the circuit board of a ws2812
module ws2812() {
    translate ([-9.7/2,-9.7/2,0]) { 
        intersection() {
            translate([9.7/2,9.7/2,-2]) cylinder(r=5.3,h=2.8+2,$fn=50);
            translate([0,0,-2]) cube([9.7,9.7,2+1.2]); // size of pcb
        }
        translate([9.7/2,9.7/2,2.9/2]) cube([5.6,5.2,2.9],center=true);    // 5050 but we have to clear the solder lugs so do 60,52
        translate([9.7/2,9.7/2,2.9/2]) cube([6.0,5.2,5],center=true);
        translate([-10,1.3,-2]) cube([30,8,2]);
        // cap and resistor
        translate([9.7/2,1.25,2.0/2]) cube([3.3,1.5,2],center=true);
        translate([9.7/2,9.7-1.25,2.0/2]) cube([3.3,1.5,2.0],center=true);
    }
}

// Generate a negative for it to fit into
module ws2812unit() {    
    difference() {
        translate([-6,-6,-2]) cube([12,12,4.3]);
        ws2812();
    }
}

module straight(start,slen) {
        translate([start,-6,1.3]) { 
            cube([slen,12,1]);
            translate([0,0,-3.3])cube([slen,1,3.3]);
            translate([0,11,-3.3])cube([slen,1,3.3]);
        }    
}

//translate([-10,12,0]) cube([270,1,1]);

difference() {
    union() {
        // curved front to tray
        intersection() {
        translate([-12,-6,-2])cube([6,12,4.3]);
        translate([-6,6,2])rotate([90,0,0])cylinder(h=12,r=4,$fn=100);
        }
        ws2812unit();   

        *union () {  // a 6 equalish spread piece
        gap = 37;
        straight(6,gap);
        l2 = gap+12;
        translate([l2,0,0])    ws2812unit();   
        straight(6+l2,gap);
        l3 = l2 + gap+12;
        translate([l3,0,0])    ws2812unit();   
        straight(6+l3,gap);
        l4 = l3 + gap+12;
        translate([l4,0,0])    ws2812unit();          
        straight(6+l4,gap);
        l5 = l4 + gap+12;
        translate([l5,0,0])    ws2812unit();     
        straight(6+l5,gap);
        l6 = l5 + gap+12;
        translate([l6,0,0])    ws2812unit();    
        straight(6+l6,270-l6-6-10);
        }
        
        union () {  // a 7 equalish spread piece
        gap = 28;
        straight(6,gap);
        l2 = gap+12;
        translate([l2,0,0])    ws2812unit();   
        straight(6+l2,gap);
        l3 = l2 + gap+12;
        translate([l3,0,0])    ws2812unit();   
        straight(6+l3,gap);
        l4 = l3 + gap+12;
        translate([l4,0,0])    ws2812unit();          
        straight(6+l4,gap);
        l5 = l4 + gap+12;
        translate([l5,0,0])    ws2812unit();     
        straight(6+l5,gap);
        l6 = l5 + gap+12;
        translate([l6,0,0])    ws2812unit();    
        straight(6+l6,gap);
        l7 = l6 + gap+12;
        translate([l7,0,0])    ws2812unit();                
        straight(6+l7,270-l7-6-10);
        }        
   }
    // smooth the top corners
    union() {
        translate([-40,-5,-1]) difference() {
            translate([0,-1,-1]) cube([300,1,1]);
            rotate([90,0,90]) cylinder(r=1,h=300,$fn=100);
        }
        translate([-40,5,-1]) difference() {
            translate([0,0,-1]) cube([300,1,1]);
            rotate([90,0,90]) cylinder(r=1,h=300,$fn=100);
        }
    }
}

*translate([10,2,0]) rotate([180,0,0])linear_extrude(height=.15) {
    text("Yui 2018",size=5,$fn=16);
}

