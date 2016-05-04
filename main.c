/*
 *      main.c
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
#include "solunar.h"

int main ()
{
	int year, month, day;
	int tz;					// time zone
	double underlong, ourlong, ourlat, zone, JD;
	double date;			// modified julian date, days since 1858 Nov 17 00:00, 1858 Nov 17 00:00 is JD 2400000.5
	double UT;
	double moonrise, moontransit, moonunder, moonset, moonphase, sunrise, suntransit, sunset;
	int object = 0;         //1 IS MOON, 0 IS SUN
	int soldayscale, phasedayscale;
	double minorstart1, minorstop1, minorstart2, minorstop2;
	double majorstart1, majorstop1, majorstart2, majorstop2;
	char mnst1[6] = "00:00", mnsp1[6] = "00:00", mnst2[6] = "00:00", mnsp2[6] = "00:00";
	char mjst1[6] = "00:00", mjsp1[6] = "00:00", mjst2[6] = "00:00", mjsp2[6] = "00:00";
//******************************************************************************

//******************************************************************************
//get date and position from user.

	printf("\n\n");
	printf( "Input Year ( yyyy ) : " );
	scanf( "%d", &year );
	printf( "Input Month ( mm ) : " );
	scanf( "%d", &month );
	printf( "Input Day ( dd ) : " );
	scanf( "%d", &day );
	printf( "Input Longitude (- is west) : " );
	scanf( "%lf", &underlong );
	printf( "Input Latitude (- is south) : " );
	scanf( "%lf", &ourlat );
	printf( "Input Time zone offset ( EST = -5 ) : " );
	scanf( "%d", &tz );

    printf("\n\nSolunar data for ");
    printf ("%d / %d / %d\n", year, month, day);

//init some values
    UT = 0.0;
	ourlong = 0.0 - underlong;  //equations use east longitude negative convention
	zone = tz / 24.0;
	JD = get_julian_date (year, month, day, UT);
	date = (JD - 2400000.5 - zone);
//get rise set times for moon and sun
	object = 1;	//sun
	get_rst ( object, date, ourlong, ourlat, &sunrise, &sunset, &suntransit);
	object = 0;	//moon
	get_rst ( object, date, ourlong, ourlat, &moonrise, &moonset, &moontransit);
//get moon under-foot time
	moonunder = get_underfoot(date, underlong);
//get moon phase
	moonphase = get_moon_phase (date);
//get solunar minor periods
	//only calculate if the minor periods do not overlap prev or next days
	if (moonrise >= 1.0 & moonrise <= 23.0) {
		sol_get_minor1(mnst1, mnsp1, moonrise);
	}
	if (moonset >= 1.0 & moonset <= 23.0) {
		sol_get_minor2(mnst2, mnsp2, moonset);
	}
	sol_display_minors (mnst1, mnsp1, mnst2, mnsp2, moonrise, moonset);
//get solunar major periods
	//only calculate if the major periods do not overlap prev and next days*/
	//if (moontransit >= 1.5 & moontransit <= 22.5) {
		sol_get_major1(mjst1, mjsp1, moontransit);
	//}
	//if (moonunder >= 1.5 & moonunder <= 22.5) {
		sol_get_major2(mjst2, mjsp2, moonunder);
	//}
	sol_display_majors (mjst1, mjsp1, mjst2, mjsp2, moontransit, moonunder);
//get day scale
	phasedayscale = phase_day_scale (moonphase);
	soldayscale = sol_get_dayscale (moonrise, moonset, moontransit, moonunder, sunrise, sunset);
	sol_display_dayscale (soldayscale, phasedayscale);
/*thats it, we are done*/
	printf("\n");
return 0;
}
