/*
 *      solunar.h
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

//astro functions
double get_julian_date(int year, int month, int day, double UT);
float ipart (double x);
double fpart (double x);
double sinalt (int object, double mjd0, int hour, double ourlong, double cphi,
                double sphi );
double lmst (double mjd, double ourlong);
double get_transit (int object, double mjd0, int hour, double ourlong);
void get_sun_pos (double t, double *ra, double *dec);
void get_moon_pos (double t, double *ra, double *dec);
void quad (double ym, double y0, double yp, double *xe, double *ye, double *z1,
                double *z2, int *nz);
void display_event_time (double time, char event [6]);
double get_moon_phase (double date);
double get_underfoot (double date, double underlong);
int get_rst (int object, double date, double ourlong, double ourlat,
                double *Rise, double *Set, double *Transit);
double get_sun_long (double t);
double get_moon_long (double t);

//solunar functions
void sol_display_minors (char mnst1[6], char mnsp1[6], char mnst2[6],
                char mnsp2[6], double moonrise, double moonset);
void sol_display_majors (char mjst1[6], char mjsp1[6], char mjst2[6],
                char mjsp2[6], double moontransit, double moonunder);
void sol_get_minor1 (char mnst1[6], char mnsp1[6], double moonrise);
void convert_time_to_string (char stringtime[6], double doubletime);
void sol_get_minor2 (char mnst2[6], char mnsp2[6], double moonset);
void sol_get_major2(char mjst2[6], char mjsp2[6], double moontransit);
void sol_get_major1(char mjst1[6], char mjsp1[6], double moontransit);
int phase_day_scale (double moonphase);
int sol_get_dayscale (double moonrise, double moonset, double moontransit,
                double moonunder, double sunrise, double sunset);
void sol_display_dayscale (int soldayscale, int phasedayscale);
