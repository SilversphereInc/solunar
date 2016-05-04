/*
 *      sol.c
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
#include <math.h>
#include "solunar.h"

void sol_get_minor1 (char mnst1[6], char mnsp1[6], double moonrise)
{
    double minorstart1, minorstop1;
    minorstart1 = moonrise - 1.0;
    minorstop1 = moonrise + 1.0;
    convert_time_to_string (mnst1, minorstart1);
    convert_time_to_string (mnsp1, minorstop1);
return;
}
//******************************************************************************

void sol_get_minor2 (char mnst2[6], char mnsp2[6], double moonset)
{
    double minorstart2, minorstop2;
    minorstart2 = moonset - 1.0;
    minorstop2 = moonset + 1.0;
    convert_time_to_string (mnst2, minorstart2);
    convert_time_to_string (mnsp2, minorstop2);
return;
}
//******************************************************************************

void sol_get_major1(char mjst1[6], char mjsp1[6], double moontransit)
{
    double majorstart1, majorstop1;
    majorstart1 = moontransit - 1.0;
    majorstop1 = moontransit + 2.0;
    convert_time_to_string (mjst1, majorstart1);
    convert_time_to_string (mjsp1, majorstop1);
return;
}
//******************************************************************************


void sol_get_major2(char mjst2[6], char mjsp2[6], double moontransit)
{
    double majorstart2, majorstop2;
    majorstart2 = moontransit - 1.0;
    majorstop2 = moontransit + 2.0;
    convert_time_to_string (mjst2, majorstart2);
    convert_time_to_string (mjsp2, majorstop2);
return;
}

//******************************************************************************

void sol_display_majors (char mjst1[6], char mjsp1[6], char mjst2[6],
            char mjsp2[6], double moontransit, double moonunder)
{
/*display earlier major time first*/
    if (moontransit < moonunder){
        printf ("major: [{start: %s, stop: %s},{start: %s, stop: %s}]", mjst1, mjsp1, mjst2, mjsp2);
    }
    else {
        printf ("major: [{start: %s, stop: %s},{start: %s, stop: %s}]", mjst2, mjsp2, mjst1, mjsp1);
    }
return;
}
//******************************************************************************

void convert_time_to_string (char stringtime[6], double doubletime)
{
    double i, d;
/*split the time into hours (i) and minutes (d)*/
    d = modf(doubletime, &i);
    d = d * 60;
    if (d >= 59.5) {
        i = i + 1;
        d = 0;
    }
/*convert times to a string*/
    if (d < 9.5) {
        snprintf(stringtime, 6, "%.0f:0%.0f",i , d);
    }
    else {
        snprintf(stringtime, 6, "%.0f:%.0f",i , d);
    }
return;
}
//******************************************************************************

void sol_display_minors (char mnst1[6], char mnsp1[6], char mnst2[6],
            char mnsp2[6], double moonrise, double moonset)
{
/*display earlier minor time first*/
    if (moonrise < moonset){
        printf ("minor: [{start: %s, stop: %s},{start: %s, stop: %s}]", mnst1, mnsp1, mnst2, mnsp2);
    }
    else {
        printf ("minor: [{start: %s, stop: %s},{start: %s, stop: %s}]", mnst2, mnsp2, mnst1, mnsp1);
    }
return;
}
//******************************************************************************

int phase_day_scale (double moonphase)
{
    int scale = 0;
    if( fabs(moonphase) <  0.9 ) {		//new
        scale = 3;
    }
    else if( fabs(moonphase) <  6.0 ) {
        scale = 2;
    }
    else if( fabs(moonphase) <  9.9 ) {
        scale = 1;
    }
    else if( fabs(moonphase) > 99 ) {		//full
        scale = 3;
    }
    else if( fabs(moonphase) > 94 ) {
        scale = 2;
    }
    else if( fabs(moonphase) > 90.1 ) {
        scale = 1;
    }
    else {
        scale = 0;
    }

return scale;
}
//******************************************************************************

int sol_get_dayscale (double moonrise, double moonset, double moontransit,
            double moonunder, double sunrise, double sunset)
{
int locsoldayscale = 0;
//check minor1 and sunrise
if ((sunrise >= (moonrise - 1.0)) && (sunrise <= (moonrise + 1.0))){
    locsoldayscale++;
}
//check minor1 and sunset
if ((sunset >= (moonrise - 1.0)) && (sunset <= (moonrise + 1.0))){
    locsoldayscale++;
}
//check minor2 and sunrise
if ((sunrise >= (moonset - 1.0)) && (sunrise <= (moonset + 1.0))){
    locsoldayscale++;
}
//check minor2 and sunset
if ((sunset >= (moonset - 1.0)) && (sunset <= (moonset + 1.0))){
    locsoldayscale++;
}
//check major1 and sunrise
if ((sunrise >= (moontransit - 2.0)) && (sunrise <= (moontransit + 2.0))){
    locsoldayscale++;
}
//check major1 and sunset
if ((sunset >= (moontransit - 2.0)) && (sunset <= (moontransit + 2.0))){
    locsoldayscale++;
}
//check major2 and sunrise
if ((sunrise >= (moonunder - 2.0)) && (sunrise <= (moonunder + 2.0))){
    locsoldayscale++;
}
//check major2 and sunset
if ((sunset >= (moonunder - 2.0)) && (sunset <= (moonunder + 2.0))){
    locsoldayscale++;
}

//catch a >2 scale, tho this shouldn't happen.
if (locsoldayscale > 2) {
    locsoldayscale = 2;
}

return locsoldayscale;
}
/*********************************************************************/

void sol_display_dayscale (int soldayscale, int phasedayscale)
{
int dayscale;
    //printf ("\nphase scale = %d", phasedayscale);
    //printf ("\nsol scale = %d", soldayscale);
    dayscale = (soldayscale + phasedayscale);
    printf ("dayscale: %d", dayscale);
return;
}
