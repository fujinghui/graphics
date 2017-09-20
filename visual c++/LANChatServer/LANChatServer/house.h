#ifndef _HOUSE_H_
#define _HOUSE_H_
class FHouse{
public:
	int house_id;
	char house_name[32];
	

	int user_id;

	bool operator==(const FHouse& house){
		return this->house_id == house.house_id;
	}
};


#endif