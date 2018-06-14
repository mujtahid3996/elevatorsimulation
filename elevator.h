#pragma once
#include "elev_app.h"
#include "msoftcon.h"
#include <iostream>
#include <iomanip>
#include <conio.h>
#include <stdlib.h>
#include <process.h>
using namespace std;
enum direction
{
	UP, DOWN, STOP
};
const int load_time = 3;
const int spacing = 7;
const int BUF_Length = 80;

class  building;
;
class elevator
{
public:
private:
	building * ptrbuilding;
	const int car_number;
	int current_floor;
	int old_floor;
	direction current_dir;
	bool destitation[NUM_FLOORS];
	int loading_timer;
	int unloading_timer;
public:

	elevator(building*,int);
	void cartick1();
	void cartivk2();
	void car_display();
	void dest_display() const;
	void decide();
	void move();
	void get_destinations();
	int get_floor() const;
	direction get_direction() const;
	~elevator();
};

class building
{
private:
	elevator * carlist[NUM_CARS];
	int num_cars;
	bool floor_reequest[2][NUM_FLOORS];
public:
	building();
	void mastertick();
	int get_cars_floor(const int) const;
	direction get_cars_dir(const int) const;
	bool get_floor_req(const int, const int) const;
	void set_floor_req(const int, const int, const bool);
	void record_floor_reqs();
	void show_floor_reqs() const;
	~building();
};