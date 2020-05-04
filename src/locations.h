// locations.h
// - list of locations for puzzle box code
// - list of startup messages for puzzle box code 
// dmf 12.23.16
//
// - the example code below compiles, and provides a better organization 
//   for listing the GPS coordinates in the puzzle_box code than just 
//   commenting out a bunch of changes over the years, I think. 
// - also note that can define startup messages and assign them
//   as below. 
//   -------------------------------------------------------
//   #include <stdio.h>
//   #define TESTING 30.123, 40.591
//   #define THEWORDS "HELLO","WORLD"
//   int main() {
//      double test[] = {TESTING}; 
//      char*  initialtext[] = {THEWORDS}; 
//      printf("  %f \n",test[0]); 
//      printf("  %f \n",test[1]); 
//      printf(" .%s.%s.\n", initialtext[0],initialtext[1]);
//    }
//   -------------------------------------------------------
//

/* WHERE TO GO! */
#define Home_61_W_15th_St        41.861210, -87.629950
#define Johns_House              39.044616, -77.080903
#define Moms_Apt                 35.794265, -79.094475
#define Gordos_House             33.907310, -84.303637
#define FLA_Rental_House         29.757492, -85.402249
#define FLA_Rental_Beach         29.757464, -85.402731
#define Cape_San_Blas_Lighthouse 29.671184, -85.356353
#define Cape_Lighthouse_Gate     29.670841, -85.356723
#define Secluded_Dunes_Entrance  29.754409, -85.400746
#define Port_St_Joe_Marina       29.815290, -85.309128
#define Ward_Bldg_NWU            41.896667, -87.619636
#define Parking_Garage_NWU       41.893844, -87.619955
#define FLA_2012_Rental          29.759048, -85.402342
#define FLA_2012_Beach           29.758973, -85.402863
#define FLA_2012_Beach_North     29.761295, -85.403606
#define FLA_2012_Cutoff_Road     29.756177, -85.400591
#define FLA_2012_Coneheads       29.743699, -85.395304
#define FLA_2012_Park_Bridge     29.766440, -85.404239
#define FLA_2016_BeachFront      29.759116, -85.403279
#define FLA_2016_Driveway         29.759162, -85.402045
#define FLA_2016_ParkAccess       29.763615,  -85.403809 
#define NORTH_POLE_AK             64.754684, -147.343647
#define TRIPLE_TAILS              29.742643, -85.301582 
#define FLA_2016_SOUTH_TIP       29.676633, -85.362699

/* STARTUP MESSAGES */ 
// note: have to be a pair of 8-character strings
// also, will setup code to print two messages at startup
#define StartupMessage2016A	" HERE I ", " AM!    "
#define StartupMessage2016B     " IT'S   ", " MAGIC  " 
#define StartupMessage2016E	" HELLO  ", " Y'ALL! "
#define StartupMessage2016F     " WHERE  ", " TO???  " 
#define StartupMessage2016C	" HELLO  ", " WORLD  "
#define StartupMessage2016D     "  I'M   ", " MAGIC  " 
// older messages
#define  oldStartupMessage1 	"   HI! ", "I'm your"
#define  oldStartupMessage2 	" MAGIC ", "  BOX!  "
#define  oldStartupMessageA1 	"Take me","on the"
#define  oldStartupMessageA2	"BEACH!"," "
#define  oldStartupMessageB1    "Go on","the road"
#define  oldStartupMessageB2	"to the","pool!"
#define  oldStartupMessageC1    "This one", "is easy!"
#define  oldStartupMessageD1    " Big    "," rocks! "
#define  oldStartupMessageE1    "I won't","open..."
#define  oldStartupMessageE2    "unless","you're"
#define  oldStartupMessageE3    "at the","right"
#define  oldStartupMessageE4    "place!"," "
