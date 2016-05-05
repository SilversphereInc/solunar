/*
 *      astro.c
 *
 *      Copyright 2008 Douglas Bacon <dugbdev@users.sourceforge.net>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "solunar.h"

//Some usefull things
#define PI			3.141592654
#define twoPI		( 2 * PI )
#define RADEG		( 180.0 / PI )
#define DEGRAD		( PI / 180.0 )
#define sind(x)  sin((x)*DEGRAD)
#define cosd(x)  cos((x)*DEGRAD)


int get_rst (int object, double date, double ourlong, double ourlat,
			double *Rise, double *Set, double *Transit)
//get rise, set and transit times of object sun or moon
{
	double sl;          //sin lat
	double cl;          //cos lat
	double xe, ye, z1, z2;
	double riseTime = 0, setTime = 0, transitTime = 0;
	double above = 0;
	double ym = 0, y0, yp;
	double sinho[2];        //0 IS MOON, 1 IS SUN
	int doesRise = 0, doesSet = 0, doesTransit = 0, hour = 1, check = 0;
	int nz = 0;
	char eventName[8];
//******************************************************************************
// init some values
	sl = sind(ourlat); cl = cosd(ourlat);  //sin and cos of latitude
	sinho[0] = .002327;         //moonrise - average diameter used
	sinho[1] = -0.014544;       //sunrise - classic value for refraction

	ym = sinalt(object, date, hour - 1, ourlong, cl, sl) - sinho[object];
	if (ym > 0) {
		above = 1;
	}
	else {
		above = 0;
	}
//start rise-set loop
	do
	{
		y0 = sinalt(object, date, hour, ourlong, cl, sl) - sinho[object];
		yp = sinalt(object, date, hour + 1, ourlong, cl, sl) - sinho[object];
		xe = 0;
		ye = 0;
		z1 = 0;
		z2 = 0;
		nz = 0;
		quad (ym, y0, yp, &xe, &ye, &z1, &z2, &nz);
		switch (nz)
		{
			case 0:	//nothing  - go to next time slot
				break;
			case 1:                      //simple rise / set event
				if (ym < 0) {       //must be a rising event
					riseTime = hour + z1;
					doesRise = 1;
				}
				else {	//must be setting
					setTime = hour + z1;
					doesSet = 1;
				}
				break;
			case 2:                      //rises and sets within interval
				if (ye < 0) {       //minimum - so set then rise
					riseTime = hour + z2;
					setTime = hour + z1;
				}
				else {    //maximum - so rise then set
					riseTime = hour + z1;
					setTime = hour + z2;
				}
				doesRise = 1;
				doesSet = 1;
				break;
				}

		ym = yp;     //reuse the ordinate in the next interval
		hour = hour + 2;
		check = (doesRise * doesSet);
	}
	while ((hour != 25) && (check != 1));
	// end rise-set loop
//GET TRANSIT TIME
	hour = 0; //reset hour
	transitTime = get_transit(object, date, hour, ourlong);
	if (transitTime != 0.0) {
		doesTransit = 1;
	}
	if (object == 0){
		printf("\"moon\":{");
	}
	else {
		printf("\"sun\":{");
	}
	//logic to sort the various rise, transit set states
	// nested if's...sorry
	if ((doesRise == 1) || (doesSet == 1) || (doesTransit == 1)) {   //current object rises, sets or transits today
		if (doesRise == 1) {
			*Rise = riseTime;
			snprintf(eventName, 6, "rise");
			display_event_time(riseTime, eventName);
			if(doesTransit == 1 || doesSet == 1){
				printf(",");
			}
		}
		else {
			*Rise = 0.0;
		}
		if (doesTransit == 1) {
			*Transit = transitTime;
			snprintf(eventName, 8, "transit");
			display_event_time(transitTime, eventName);
			if(doesSet == 1){
				printf(",");
			}
		}
		else {
			*Transit = 0.0;
		}
		if (doesSet == 1) {
			*Set = setTime;
			snprintf(eventName, 6, "set");
			display_event_time(setTime, eventName);
		}
		else {
			*Set = 0.0;
		}
		printf("},");

	}
	else { //current object not so simple
		if (above == 1) {
			//printf ("\nalways above horizon");
		}
		else {
			//printf ("\nalways below horizon");
		}
	}
	//thats it were done.
return 0;
}

/******************************************************************************/
double get_julian_date(int year, int month, int day, double UT)
{
	double locJD, a, b, c, d, e, f, g;
	a = modf((month + 9)/12, &b);
	a = modf((7 * (year + b))/4, &c);
	a = modf((275 * month)/9, &d);
	e = 367 * year - c + d + day + 1721013.5 + UT/24;
	f = (100 * year + month - 190002.5);
	g = f/fabs(f);
	locJD = e - 0.5 * g + 0.5;
return (locJD);
}

/******************************************************************************/
void display_event_time (double time, char event [6])
{
	char sTime[6];
	convert_time_to_string (sTime, time);
	printf("\"%s\":\"%s\"",event, sTime);
return;
}

/******************************************************************************/
float ipart (double x)
//returns the true integer part, even for negative numbers
{
	float a;
	if (x != 0) {
	a = x/fabs(x) * floor(fabs(x));
	}
	else {
	    a=0;
	}
return a;
}

/******************************************************************************/
double fpart (double x)
//returns fractional part of a number
{
	x = x - floor(x);
	if ( x < 0) {
		x = x + 1;
	}
return x;
}

/******************************************************************************/
double sinalt (int object, double mjd0, int hour, double ourlong, double cphi,
            double sphi )
/*
returns sine of the altitude of either the sun or the moon given the modified
julian day number at midnight UT and the hour of the UT day, the longitude of
the observer, and the sine and cosine of the latitude of the observer
*/
{
	double loc_sinalt;   //sine of the altitude, return value;
	double ra = 0.0;
	double dec = 0.0;
	double instant, t;
	double lha;			//hour angle
	instant = mjd0 + hour / 24.0;
	t = (instant - 51544.5) / 36525;
	if (object == 0) {
		get_moon_pos (t, &ra, &dec);
	}
	else {
		get_sun_pos (t, &ra, &dec);
	}
	lha = 15.0 * (lmst(instant, ourlong) - ra);    //hour angle of object
	loc_sinalt = sphi * sind(dec) + cphi * cosd(dec) * cosd(lha);
return (loc_sinalt);
}

/******************************************************************************/
double lmst (double mjd, double ourlong)
//returns the local siderial time for the modified julian date and longitude
{
	double value;
	float mjd0;
	double ut;
	double t;
	double gmst;
	mjd0 = ipart(mjd);
	ut = (mjd - mjd0) * 24;
	t = (mjd0 - 51544.5) / 36525;
	gmst = 6.697374558 + 1.0027379093 * ut;
	gmst = gmst + (8640184.812866 + (.093104 - .0000062 * t) * t) * t / 3600;
	value = 24.0 * fpart((gmst - ourlong / 15.0) / 24.0);
return (value);
}

/******************************************************************************/
void get_sun_pos (double t, double *ra, double *dec)
/*
Returns RA and DEC of Sun to roughly 1 arcmin for few hundred years either side
of J2000.0
*/
{
	double COSEPS = 0.91748;
	double SINEPS = 0.39778;
	double m, dL, L, rho, sl;
	double RA, DEC;
	double x, y, z;
	m = twoPI * fpart(0.993133 + 99.997361 * t);        //Mean anomaly
	dL = 6893 * sin(m) + 72 * sin(2 * m);          //Eq centre
	L = twoPI * fpart(0.7859453 + m / twoPI + (6191.2 * t + dL) / 1296000);
	sl = sin(L);
	x = cos(L);
	y = COSEPS * sl;
	z = SINEPS * sl;
	rho = sqrt(1 - z * z);
	DEC = (360 / twoPI) * atan2(z , rho);
	RA = (48 / twoPI) * atan2(y , (x + rho));
	if (RA < 0) {
		RA = RA + 24;
	}
	*ra = RA;
	*dec = DEC;
return;
}

/******************************************************************************/
void get_moon_pos (double t, double *ra, double *dec)
/*
returns ra and dec of Moon to 5 arc min (ra) and 1 arc min (dec) for a few
centuries either side of J2000.0 Predicts rise and set times to within minutes
for about 500 years in past - TDT and UT time diference may become significant
for long times
*/
{
	double ARC = 206264.8062;
	double COSEPS = 0.91748;
	double SINEPS = 0.39778;
	double L0, L, LS, d, F;
	L0 = fpart(.606433 + 1336.855225 * t);    //'mean long Moon in revs
	L = twoPI * fpart(.374897 + 1325.55241 * t); //'mean anomaly of Moon
	LS = twoPI * fpart(.993133 + 99.997361 * t); //'mean anomaly of Sun
	d = twoPI * fpart(.827361 + 1236.853086 * t); //'diff longitude sun and moon
	F = twoPI * fpart(.259086 + 1342.227825 * t); //'mean arg latitude
	//' longitude correction terms
	double dL, h;
	dL = 22640 * sin(L) - 4586 * sin(L - 2 * d);
	dL = dL + 2370 * sin(2 * d) + 769 * sin(2 * L);
	dL = dL - 668 * sin(LS) - 412 * sin(2 * F);
	dL = dL - 212 * sin(2 * L - 2 * d) - 206 * sin(L + LS - 2 * d);
	dL = dL + 192 * sin(L + 2 * d) - 165 * sin(LS - 2 * d);
	dL = dL - 125 * sin(d) - 110 * sin(L + LS);
	dL = dL + 148 * sin(L - LS) - 55 * sin(2 * F - 2 * d);
	//' latitude arguments
	double S, N, lmoon, bmoon;
	S = F + (dL + 412 * sin(2 * F) + 541 * sin(LS)) / ARC;
	h = F - 2 * d;
	//' latitude correction terms
	N = -526 * sin(h) + 44 * sin(L + h) - 31 * sin(h - L) - 23 * sin(LS + h);
	N = N + 11 * sin(h - LS) - 25 * sin(F - 2 * L) + 21 * sin(F - L);
	lmoon = twoPI * fpart(L0 + dL / 1296000); //  'Lat in rads
	bmoon = (18520 * sin(S) + N) / ARC;  //     'long in rads
	//' convert to equatorial coords using a fixed ecliptic
	double CB, x, V, W, y, Z, rho, DEC, RA;
	CB = cos(bmoon);
	x = CB * cos(lmoon);
	V = CB * sin(lmoon);
	W = sin(bmoon);
	y = COSEPS * V - SINEPS * W;
	Z = SINEPS * V + COSEPS * W;
	rho = sqrt(1.0 - Z * Z);
	DEC = (360.0 / twoPI) * atan2(Z , rho);
	RA = (48.0 / twoPI) * atan2(y , (x + rho));
	if (RA < 0) {
		RA = RA + 24.0;
	}
	*ra = RA;
	*dec = DEC;
return;
}

/******************************************************************************/
void quad (double ym, double y0, double yp, double *xe, double *ye, double *z1,
            double *z2, int *nz)
/*
finds a parabola through three points and returns values of coordinates of
extreme value (xe, ye) and zeros if any (z1, z2) assumes that the x values are
-1, 0, +1
*/
{
	double a, b, c, dx, dis, XE, YE, Z1, Z2;
	int NZ;
	NZ = 0;
	XE = 0;
	YE = 0;
	Z1 = 0;
	Z2 = 0;
	a = .5 * (ym + yp) - y0;
	b = .5 * (yp - ym);
	c = y0;
	XE = (0.0 - b) / (a * 2.0); //              'x coord of symmetry line
	YE = (a * XE + b) * XE + c; //      'extreme value for y in interval
	dis = b * b - 4.0 * a * c;   //    'discriminant
	//more nested if's
		if ( dis > 0.000000 ) {                 //'there are zeros
		dx = (0.5 * sqrt(dis)) / (fabs(a));
		Z1 = XE - dx;
		Z2 = XE + dx;
		if (fabs(Z1) <= 1) {
			NZ = NZ + 1 ;   // 'This zero is in interval
		}
		if (fabs(Z2) <= 1) {
			NZ = NZ + 1  ;   //'This zero is in interval
		}
		if (Z1 < -1) {
			Z1 = Z2;
		}
	}
	*xe = XE;
	*ye = YE;
	*z1 = Z1;
	*z2 = Z2;
	*nz = NZ;
return;
}

/******************************************************************************/
double get_moon_phase (double date)
{
    int PriPhaseOccurs; //1 = yes, 0 = no
    int i = 0;
    double ourhour;
    double hour = -1;
    double ls, lm, diff;
    double instant, t, ra, dec;
    double phase;
    double hourarray[100];
    double minarray[119];
    double illumin, age, PriPhaseTime;
    char PhaseName[16];
/*some notes on structure of hourarray[]
 *  increment is 15mins
 * i =  0, hourarray[0] = hour -1, hour 23 of prev day.
 * i =  1, hourarry[1] = hour -0.75, hour 23.15 of prev day.
 * i = 4, hourarray[4] = hour 0 of today.
 * i = 52, hourarray[52] = hour 12 of today.
 * i = 99, hourarray[99] = hour 23.75 of today.
 * i = 100, hourarray[100] = hour 0 of nextday.
 * 
 * to convert i to todays hour = (i/4 -1)
 */

//find and store illumination for every 1/4 hour in an array
while (i < 104)
{
    instant = date + hour / 24.0;
    t = (instant - 51544.5) / 36525;
    lm = get_moon_long (t);
    ls = get_sun_long (t);
    diff = lm - ls;
	phase = (1.0 - cosd(lm - ls))/2;
	phase *=100;
	if (diff < 0) {
		diff += 360;
	}
	if (diff > 180) {
		phase *= -1;
	}
    illumin = fabs(phase);
    hourarray[i] = illumin;
    i++;
    hour+= 0.25;
}
i = 0;
while (i < 104)
{
    ourhour = i;
    ourhour = ((ourhour/4) - 1);
    //check for a new moon
    if ((hourarray[i] < hourarray[i+1]) && (hourarray[i] < 0.001)) {
       break;
    }
    //check for a full moon
    if ( (hourarray[i] > hourarray[i+1]) && (hourarray[i] > 99.9999) ){
        break;
    }
    //check for a first quarter
    if ( (hourarray[i] < hourarray[i+1]) && (hourarray[i] > 50) && (hourarray[i] < 50.5)){
        break;
    }
    //check for a last quarter
    if ( (hourarray[i] > hourarray[i+1]) && (hourarray[i] < 50) && (hourarray[i] > 49.5) ){
        break;
    }

    i++;
}
if ( ourhour < 0 || ourhour >= 24 ) {
    PriPhaseOccurs = 0;
}
else {
    PriPhaseOccurs = 1;
}

if (PriPhaseOccurs == 1){
    //check every min start with the previous hour
    if (ourhour > 0) {
    	   hour = ipart(ourhour) - 1;
    }
    else {
   hour = ipart(ourhour);
    }

    PriPhaseTime = hour;
    i = 0;
    while (i < 120)
    {
        instant = date + hour / 24.0;
        t = (instant - 51544.5) / 36525;
        lm = get_moon_long (t);
        ls = get_sun_long (t);
        diff = lm - ls;
        phase = (1.0 - cosd(lm - ls))/2;
        phase *=100;
        if (diff < 0) {
            diff += 360;
        }
        if (diff > 180) {
            phase *= -1;
        }
// we are getting age at the wrong time here, maybe for a primary phase
// we should use a static age, like we do for illumin.        
        //age = fabs(diff/13);
        illumin = fabs(phase);
        minarray[i] = illumin;
        hour = hour + 0.016667;
        i++;
    }

    i = 0;
    int ourmin;
    while (i < 120)
    {
        ourmin = i;
        //check for a new moon
        if ((minarray[i] < minarray[i+1]) && (minarray[i] < 0.1)) {
            illumin = 0;
            age = 0.0;
            snprintf(PhaseName, 16, "new");
            break;
        }
        //check for a full moon
        if ( (minarray[i] > minarray[i+1]) && (minarray[i] > 99) ){
            illumin = 100;
            age = 14.0;
            snprintf(PhaseName, 16, "full");
            break;
        }
        //check for a first quarter
        if ( (minarray[i] < minarray[i+1]) && (minarray[i] > 50) && (minarray[i] < 51)){
            illumin = 50;
            age = 7.0;
            snprintf(PhaseName, 16, "first quarter");
            break;
        }
        //check for a last quarter
        if ( (minarray[i] > minarray[i+1]) && (minarray[i] < 50) && (minarray[i] > 49) ){
            illumin = 50;
            age = 21.0;
            snprintf(PhaseName, 16, "last quarter");
            break;
        }
        PriPhaseTime = PriPhaseTime + 0.016667;
        i++;
    }

}
else {
//if we didn't find a primary phase, check the phase at noon.
//    date = (JD - 2400000.5);
    instant = date + .5;//check at noon
    t = (instant - 51544.5) / 36525;
    lm = get_moon_long (t);
    ls = get_sun_long (t);
    diff = lm - ls;
    phase = (1.0 - cosd(lm - ls))/2;
    phase *=100;
    if (diff < 0) {
        diff += 360;
    }
    if (diff > 180) {
        phase *= -1;
    }
    //age = fabs((lm - ls)/13);
    age = fabs(diff/13);
	illumin = fabs(phase);
//Get phase type
        if( fabs(phase) <  50 && phase < 0 ) {
			snprintf(PhaseName, 16, "waning crescent");
		}
		else if( fabs(phase) <  50 && phase > 0 ) {
			snprintf(PhaseName, 16, "waxing crescent");
		}
		else if( fabs(phase) < 100 && phase < 0 ) {
			snprintf(PhaseName, 16, "waning gibbous");
		}
		else if( fabs(phase) < 100 && phase > 0 ) {
			snprintf(PhaseName, 16, "waxing gibbous");
		}
		else {
		    snprintf(PhaseName, 16, "unknown");
		}
}
if (PriPhaseOccurs == 1){
    char sTime[6];
    convert_time_to_string (sTime, PriPhaseTime);
    printf("\"moon_phase\":\"%s\",\"moon_phase_time\":\"%s\",", PhaseName, sTime);
}
else{
    printf("\"moon_phase\":\"%s\",", PhaseName);
}
	printf("\"moon_illumination\":\"%2.1f%%\",", illumin);
    printf("\"days_since_new\":\"%2.1f\",", age);
return illumin;
}
/******************************************************************************/
double get_transit (int object, double mjd0, int hour, double ourlong)
{
	double ra = 0.0;
	double dec = 0.0;   //not used here but the pos functions return it.
	double instant, t;
	double lha;			//local hour angle
	double loc_transit;	// transit time, return value.
	int min = 0;
    int hourarray[24];
    int minarray[60];
    double LA;  //local angle
    int sLA;    //sign of angle
    double mintime;

//loop through all 24 hours of the day and store the sign of the angle in an array
//actually loop through 25 hours if we reach the 25th hour with out a transit then no transit condition today.

		while (hour < 25.0)
	{
		instant = mjd0 + hour / 24.0;
		t = (instant - 51544.5) / 36525;
		if (object == 0) {
			get_moon_pos (t, &ra, &dec);
		}
		else {
			get_sun_pos (t, &ra, &dec);
		}
		lha = (lmst(instant, ourlong) - ra);
        LA = lha * 15.04107;    //convert hour angle to degrees
        sLA = LA/fabs(LA);      //sign of angle
		hourarray[hour] = sLA;
		hour++;
	}
//search array for the when the angle first goes from negative to positive
		int i = 0;
		while (i < 25)
        {
            loc_transit = i;
            if (hourarray[i] - hourarray[i+1] == -2) {
                //we found our hour
                break;
            }

            i++;
        }
//check for no transit, return zero
        if (loc_transit > 23) {
            // no transit today
            loc_transit = 0.0;
            return loc_transit;
        }

//loop through all 60 minutes of the hour and store sign of the angle in an array
	mintime = loc_transit;
	while (min < 60)
	{
		instant = mjd0 + mintime / 24.0;
		t = (instant - 51544.5) / 36525;
		if (object == 0) {
			get_moon_pos (t, &ra, &dec);
		}
		else {
			get_sun_pos (t, &ra, &dec);
		}
		lha = (lmst(instant, ourlong) - ra);
		LA = lha * 15.04107;
        sLA = (int)(LA/fabs(LA));
        minarray[min] = sLA;
		min++;
        mintime = mintime + 0.016667;		//increment 1 minute
	}

    i = 0;
	while (i < 60)
    {
        if (minarray[i] - minarray[i+1] == -2) {
        //we found our min
        break;
        }
        i++;
        loc_transit = loc_transit + 0.016667;
    }
return (loc_transit);
}

double get_underfoot (double date, double underlong)
{
	double moonunderTime;
	int doesUnderfoot;
	char eventName[11];
	moonunderTime = get_transit (0, date, 0, underlong);
	if (moonunderTime != 0.0) {
		snprintf(eventName, 11, "under_foot");
		display_event_time(moonunderTime, eventName);
		printf(",");
	}
	else {
			//printf ("\nMoon does not transit under foot");
	}

	return moonunderTime;
}

/******************************************************************************/
double get_moon_long (double t)
{
	double ARC = 206264.8062;
	double COSEPS = 0.91748;
	double SINEPS = 0.39778;
	double L0, L, LS, d, F;
	double moonlong;
	L0 = fpart(.606433 + 1336.855225 * t);    //'mean long Moon in revs
	L = twoPI * fpart(.374897 + 1325.55241 * t); //'mean anomaly of Moon
	LS = twoPI * fpart(.993133 + 99.997361 * t); //'mean anomaly of Sun
	d = twoPI * fpart(.827361 + 1236.853086 * t); //'diff longitude sun and moon
	F = twoPI * fpart(.259086 + 1342.227825 * t); //'mean arg latitude
	//' longitude correction terms
	double dL, h;
	dL = 22640 * sin(L) - 4586 * sin(L - 2 * d);
	dL = dL + 2370 * sin(2 * d) + 769 * sin(2 * L);
	dL = dL - 668 * sin(LS) - 412 * sin(2 * F);
	dL = dL - 212 * sin(2 * L - 2 * d) - 206 * sin(L + LS - 2 * d);
	dL = dL + 192 * sin(L + 2 * d) - 165 * sin(LS - 2 * d);
	dL = dL - 125 * sin(d) - 110 * sin(L + LS);
	dL = dL + 148 * sin(L - LS) - 55 * sin(2 * F - 2 * d);
	//' latitude arguments
	double S, N, lmoon, bmoon;
	S = F + (dL + 412 * sin(2 * F) + 541 * sin(LS)) / ARC;
	h = F - 2 * d;
	//' latitude correction terms
	N = -526 * sin(h) + 44 * sin(L + h) - 31 * sin(h - L) - 23 * sin(LS + h);
	N = N + 11 * sin(h - LS) - 25 * sin(F - 2 * L) + 21 * sin(F - L);
	lmoon = twoPI * fpart(L0 + dL / 1296000); //  'Lat in rads
	bmoon = (18520 * sin(S) + N) / ARC;  //     'long in rads
	moonlong = lmoon * RADEG;
return moonlong;
}

/******************************************************************************/
double get_sun_long (double t)
{
	double COSEPS = 0.91748;
	double SINEPS = 0.39778;
	double m, dL, L, rho, sl;
	double RA, DEC;
	double x, y, z;
	double sunlong;
	m = twoPI * fpart(0.993133 + 99.997361 * t);        //Mean anomaly
	dL = 6893 * sin(m) + 72 * sin(2 * m);          //Eq centre
	L = twoPI * fpart(0.7859453 + m / twoPI + (6191.2 * t + dL) / 1296000);
	sunlong = L * RADEG;
	return sunlong;
}


/******************************************************************************/
