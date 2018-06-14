#include "stdafx.h"
#include "elevator.h"
#include <iostream>
using namespace std;
elevator::elevator(building* ptrb ,int nc):ptrbuilding(ptrb),car_number(nc)
{
	current_floor = 0;
	old_floor = 0;
	current_dir = STOP;
	for (int j = 0; j < NUM_FLOORS; j++)
		destitation[j] = false;
	loading_timer = 0;
	unloading_timer = 0;

}

void elevator::cartick1()
{
	car_display();//display elevator box;
	dest_display();
	if (loading_timer)
		--loading_timer;
	if (unloading_timer)
		--unloading_timer;
	decide();
}

void elevator::cartivk2()
{
	move();
}

void elevator::car_display()
{
	set_cursor_pos(spacing + (car_number + 1)*spacing, NUM_FLOORS - old_floor);
	cout << " ";
	switch (loading_timer)
	{
	case 3:
		cout << "\x01\xDB \xDB ";
		break;
	case 2:
		cout << " \xDB\x01\xDB";
	case 1:
		cout << " \xDB\xDB\xDB";
	default:
		cout << " \xDB\xDB\xDB";
		break;
	}
	set_cursor_pos(spacing + (car_number + 1)*spacing, NUM_FLOORS - current_floor);
	switch (unloading_timer)
	{
	case 3:
		cout << "\xDB\x01\xDB ";
		break;
	case 2:
		cout << "\xDB \xDB\x01";
		break;
	case 1:
		cout << "\xDB\xDB\xDB ";
		break;
	default:
		cout << "\xDB\xDB\xDB  ";
	}
	old_floor = current_floor;
}

void elevator::dest_display() const
{
	for (int j = 0; j < NUM_FLOORS; j++)
	{
		set_cursor_pos(spacing - 2 + (car_number + 1)*spacing, NUM_FLOORS - j);
		if (destitation[j] == true)
			cout << '\xFE';
		else
			cout << ' ';
	}

}

void elevator::decide()
{
	int j;
	//flags indicate if destinations or requests above/below us
	bool destinats_above, destins_below;//destination
	bool requests_above, requests_below;//requests
	int nearest_higher_request_ = 0;
	int nearest_lower_request = 0;
	//flags indicate if there is another car,going in the same
	//direction,between us and the nearest floor request (FR)
	bool car_between_up, car_between_dn;
	bool car_opposite_up, car_opposite_dn;
	int ofloor;
	direction odir;
	if ((current_floor == NUM_FLOORS - 1 && current_dir == UP) || (current_floor == 0 && current_dir == DOWN))
			current_dir = STOP;
	//if there is a destination on this floor,unload passangers
	if (destitation[current_floor] == true)
	{
		destitation[current_floor] = false;
		if (!unloading_timer)
			unloading_timer = load_time;
		return;
	}
	if (ptrbuilding->get_floor_req(UP, current_floor) && current_dir != DOWN)
	{
		current_dir = UP;
		ptrbuilding->set_floor_req(current_dir, current_floor, false);
		if (!loading_timer)
			loading_timer = load_time;
		return;
	}
	//if there's a down floor request in this floor
	if (ptrbuilding->get_floor_req(DOWN, current_floor) && current_dir != UP)
	{
		current_dir = DOWN;
		ptrbuilding->set_floor_req(current_dir, current_floor, false);
		if (!loading_timer)
			loading_timer = load_time;
		return;
	}
	destinats_above = destins_below = false;
	requests_above = requests_below = false;
	for (j = current_floor + 1; j < NUM_FLOORS; j++)
	{
		if (destitation[j])
			destinats_above = true;
		if (ptrbuilding->get_floor_req(UP, j) || ptrbuilding->get_floor_req(DOWN, j))
		{
			requests_above = true;
			if (!nearest_higher_request_)
				nearest_higher_request_ = j;
		}
	}
	for (j = current_floor - 1; j >= 0; j--)
	{
		if (destitation[j])
			destins_below = true;
		if (ptrbuilding->get_floor_req(UP, j) || ptrbuilding->get_floor_req(DOWN, j))
		{
			requests_below = true;
			if (!nearest_lower_request)
				nearest_lower_request = j;
		}
	}
	if (!destinats_above && !requests_above && !destins_below && !requests_below)
	{
		current_dir = STOP;
		return;
	}
	if (destinats_above && (current_dir == STOP || current_dir == DOWN))
	{
		current_dir = DOWN;
		return;
	}
	car_between_up = car_between_dn = false;
	car_opposite_up = car_opposite_dn = false;
	for (j = 0; j < NUM_CARS; j++)
	{
		if (j != car_number)
		{
			ofloor = ptrbuilding->get_cars_floor(j);
			odir = ptrbuilding->get_cars_dir(j);

			if ((odir == UP || odir == STOP) && requests_above)
				if ((ofloor > current_floor && ofloor <= nearest_higher_request_) || (ofloor == current_floor && j < car_number))
					car_between_up = true;
			if ((odir == DOWN || odir == STOP) && requests_below)
				if ((ofloor < current_floor && ofloor >= nearest_lower_request) || ofloor == current_floor && j < car_number)
					car_between_dn = true;
			if ((odir == UP || odir == STOP) && requests_below)
				if (nearest_lower_request >= ofloor && nearest_lower_request - ofloor < current_floor - nearest_lower_request)
					car_opposite_up = true;
			if ((odir == DOWN || odir == STOP) && requests_above)
				if (ofloor >= nearest_higher_request_ && ofloor - nearest_higher_request_ < nearest_higher_request_ - current_floor)
					car_opposite_dn = true;


		}
	}
	if ((current_dir == UP || current_dir == STOP) && requests_above && !car_between_up && !car_opposite_dn)
	{
		current_dir = UP;
		return;
	}
	if ((current_dir == DOWN || current_dir == STOP) && requests_below && !car_between_dn && !car_opposite_dn)
	{
		current_dir = DOWN;
		return;
	}
	current_dir = STOP;
}

void elevator::move()
{
	if (loading_timer || unloading_timer)
		return;
	if (current_dir == UP)
		current_floor++;
	else if (current_dir == DOWN)
		current_floor--;
}

void elevator::get_destinations()
{
	char ustring[BUF_Length];
	int dest_floor;
	set_cursor_pos(1, 22);
	clear_line();
	set_cursor_pos(1, 22);
	cout << "Car " << (car_number + 1)
		<< " has stopped at floor " << (current_floor + 1);
	for (int j = 1; j < NUM_FLOORS; j++)
	{
		set_cursor_pos(1, 24);
		cout << "Destination " << j << ": ";
		cin.get(ustring, BUF_Length);
		cin.ignore(10, '\n');
		dest_floor = atoi(ustring);
		set_cursor_pos(1, 24);
		if (dest_floor == 0)
		{
			set_cursor_pos(1, 22);
			clear_line();
			set_cursor_pos(1, 23);
			clear_line();
			set_cursor_pos(1, 24);
			clear_line();
			return;
		}
		--dest_floor;
		if (dest_floor == current_floor)
		{
			--j;
			continue;
		}
		if (j == 1 && current_dir == STOP)
			current_dir = (dest_floor < current_floor) ? DOWN : UP;
		destitation[dest_floor] = true;
		dest_display();
	}
}

int elevator::get_floor() const
{
	return current_floor;
}

direction elevator::get_direction() const
{
	return current_dir;
}


elevator::~elevator()
{

}

building::building()
{
	char ustring[BUF_Length];
	init_graphics();
	clear_screen();
	num_cars = 0;
	for (int k = 0; k < NUM_CARS; k++)
	{
		carlist[k] = new elevator(this, num_cars);//make elevators
		num_cars++;
	}
	for (int j = 0; j < NUM_FLOORS; j++)//for each floor
	{
		set_cursor_pos(3, NUM_FLOORS - j);
		_itoa_s(j + 1, ustring, 10);
		cout << setw(3) << ustring;
		floor_reequest[UP][j] = false;
		floor_reequest[DOWN][j] = false;
	}
}

void building::mastertick()
{
	int j;
	show_floor_reqs();//display floor reqs
	for (int j = 0; j < NUM_CARS; j++)
		carlist[j]->cartick1();
	for (int j = 0; j < NUM_CARS; j++)
		carlist[j]->cartivk2();
}

int building::get_cars_floor(const int carno) const
{
	return carlist[carno]->get_floor();
}

direction building::get_cars_dir(const int carno) const
{
	return carlist[carno]->get_direction();
}

bool building::get_floor_req(const int, const int) const
{
	return false;
}

void building::set_floor_req(const int dir, const int floor, const bool updown)
{
	floor_reequest[dir][floor]=updown;
}

void building::record_floor_reqs()
{
	char ch = 'x';//utility char for input
	char ustring[BUF_Length];//utility string for input
	int ifloor;
	char chDIrection;
	set_cursor_pos(1, 22);
	cout << "Press [ENter] to Call an elevator: ";
	if (!_kbhit())
		return;
	cin.ignore(10, '\n');
	if (ch == '\x1B')
		exit(0);
	set_cursor_pos(1, 22);
	set_cursor_pos(1, 22);
	cout << "Enter the floor You're on: ";
	cin.get(ustring, BUF_Length);
	cin.ignore(10, '\n');//eat chars,including newline
	ifloor = atoi(ustring);
	cout << "enter direction you want to go(u or d):";
	cin.get(chDIrection);
	cin.ignore(10, '\n');
	if (chDIrection == 'U' || chDIrection == 'u')
		floor_reequest[UP][ifloor - 1] = true;
	if (chDIrection == 'd' || chDIrection == 'D')
		floor_reequest[DOWN][ifloor - 1] = true;
	set_cursor_pos(1, 22); clear_line();
	set_cursor_pos(1, 23); clear_line();
	set_cursor_pos(1, 24); clear_line();
}

void building::show_floor_reqs() const
{
	for (int j = 0; j < NUM_FLOORS; j++)
	{
		set_cursor_pos(spacing, NUM_FLOORS - j);
		if (floor_reequest[UP][j] == TRUE)
			cout << '\x1E';
		else
			cout << ' ';
		set_cursor_pos(spacing + 3, NUM_FLOORS - j);
		if (floor_reequest[DOWN][j] == true)
			cout << '\x1F';
		else
			cout << ' ';

	}
}

building::~building()
{
	for (int k = 0; k < NUM_CARS; k++)
		delete carlist[k];
}
