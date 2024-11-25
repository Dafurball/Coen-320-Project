
#ifndef AIRPLANE_H_
#define AIRPLANE_H_

class airplane {
public:
	airplane();
	airplane (int, int, int, int, int, int, int, int);
	virtual ~airplane();

	void set_time(int);
	void set_id(int);
	void set_coords(int);
	void set_speed(int);

	int get_time();
	int get_id();
	int get_coords();
	int get_x();
	int get_y();
	int get_z();
	int get_speedX();
	int get_speedY();
	int get_speedZ();

	int get_speed();
	void change_speed(double);
	void change_altitude(int);
	void change_direction(int, int);


	void new_location();
    static void* location_update(void* arg);

	void print();



private:

	int time;
	int id;
	int x,y,z;
	double SpeedX,SpeedY,SpeedZ;
	int delta = 1;



};


#endif /* AIRPLANE_H_ */
