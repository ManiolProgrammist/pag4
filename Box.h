//box bêdzie zwraca³ wszystkie vertexy po obliczeniu œrodka i d³ugoœci œcianki

#include <vector>
struct point {
	float p[5]{ 0,0,0,0,0 };
};
struct rec {
	point points[4];
};
using namespace std;
class Box {
public:
	float x; //x,y,z srodka
	float y;
	float z;
	float d; //dlugosc scianki
	bool generat;


	Box(float p[3], float _d) {
		x = p[0];
		y = p[1];
		z = p[2];
		d = _d;
		generat = false;
	}


	void generate(vector<Box> &boxes) {
	
		for (int _x = -1; _x < 2; _x++) {
			for (int _y = -1; _y < 2; _y++) {
				for (int _z = -1; _z < 2; _z++) {
					int sum = abs(_x) + abs(_y) + abs(_z);
					float newR = d / 3;
					if (sum > 1) {
						float p[] = { x + _x * newR, y + _y * newR, z + _z * newR };
						Box b(p, newR);
						boxes.push_back(b);
					}
				}
			}
		}
		generat = true;
	}


	void ReturnVertexes(vector<rec> &Box) {
		//œcianka przednia
		//vector<rec> Box;
		Box.push_back(rec());
		//prawo góra
		Box[0].points[0].p[0] = x + d / 2;
		Box[0].points[0].p[1] = y + d / 2;
		Box[0].points[0].p[2] = z - d / 2;
		Box[0].points[0].p[3] = 1.f;
		Box[0].points[0].p[4] = 1.f;


		//prawo dó³	
		Box[0].points[1].p[0] = x + d / 2;
		Box[0].points[1].p[1] = y - d / 2;
		Box[0].points[1].p[2] = z - d / 2;
		Box[0].points[1].p[3] = 1.f;
		Box[0].points[1].p[4] = 0.f;


		//lewo dó³
		Box[0].points[2].p[0] = x - d / 2;
		Box[0].points[2].p[1] = y - d / 2;
		Box[0].points[2].p[2] = z - d / 2;
		Box[0].points[2].p[3] = 0.f;
		Box[0].points[2].p[4] = 0.f;


		//lewo góra
		Box[0].points[3].p[0] = x - d / 2;
		Box[0].points[3].p[1] = y + d / 2;
		Box[0].points[3].p[2] = z - d / 2;
		Box[0].points[3].p[3] = 0.f;
		Box[0].points[3].p[4] = 1.f;




		Box.push_back(rec());
		//back
		for (int i = 0;i < 4;i++) {
			Box[1].points[i].p[0] = Box[0].points[i].p[0];
			Box[1].points[i].p[1] = Box[0].points[i].p[1];
			Box[1].points[i].p[2] = z + d / 2;
			Box[1].points[i].p[3] = Box[0].points[i].p[3];
			Box[1].points[i].p[4] = Box[0].points[i].p[4];
		}




		Box.push_back(rec());
		//left
		//prawo góra
		Box[2].points[0].p[0] = x - d / 2;
		Box[2].points[0].p[1] = y - d / 2;
		Box[2].points[0].p[2] = z - d / 2;
		Box[2].points[0].p[3] = 0.0f;
		Box[2].points[0].p[4] = 1.0f;

		//prawo dó³
		Box[2].points[1].p[0] = x - d / 2;
		Box[2].points[1].p[1] = y + d / 2;
		Box[2].points[1].p[2] = z - d / 2;
		Box[2].points[1].p[3] = 1.0f;
		Box[2].points[1].p[4] = 1.0f;

		//lewo dó³
		Box[2].points[2].p[0] = x - d / 2;
		Box[2].points[2].p[1] = y + d / 2;
		Box[2].points[2].p[2] = z + d / 2;
		Box[2].points[2].p[3] = 1.0f;
		Box[2].points[2].p[4] = 0.0f;

		//lewo góra
		Box[2].points[3].p[0] = x - d / 2;
		Box[2].points[3].p[1] = y - d / 2;
		Box[2].points[3].p[2] = z + d / 2;
		Box[2].points[3].p[3] = 0.0f;
		Box[2].points[3].p[4] = 0.0f;





		Box.push_back(rec());
		//right
		for (int i = 0;i < 4;i++) {
			Box[3].points[i].p[0] = x + d / 2;
			Box[3].points[i].p[1] = Box[2].points[i].p[1];
			Box[3].points[i].p[2] = Box[2].points[i].p[2];
			Box[3].points[i].p[3] = Box[2].points[i].p[3];
			Box[3].points[i].p[4] = Box[2].points[i].p[4];
		}





		Box.push_back(rec());
		//down
		//prawo góra
		Box[4].points[0].p[0] = x + d / 2;
		Box[4].points[0].p[1] = y - d / 2;
		Box[4].points[0].p[2] = z + d / 2;
		Box[4].points[0].p[3] = 1.0f;
		Box[4].points[0].p[4] = 0.0f;

		//prawo dó³
		Box[4].points[1].p[0] = x + d / 2;
		Box[4].points[1].p[1] = y - d / 2;
		Box[4].points[1].p[2] = z - d / 2;
		Box[4].points[1].p[3] = 1.0f;
		Box[4].points[1].p[4] = 1.0f;
		//lewo dó³
		Box[4].points[2].p[0] = x - d / 2;
		Box[4].points[2].p[1] = y - d / 2;
		Box[4].points[2].p[2] = z - d / 2;
		Box[4].points[2].p[3] = 0.0f;
		Box[4].points[2].p[4] = 1.0f;
		//lewo góra
		Box[4].points[3].p[0] = x - d / 2;
		Box[4].points[3].p[1] = y - d / 2;
		Box[4].points[3].p[2] = z + d / 2;
		Box[4].points[3].p[3] = 0.0f;
		Box[4].points[3].p[4] = 0.0f;




		Box.push_back(rec());
		//up
		for (int i = 0;i < 4;i++) {
			Box[5].points[i].p[0] = Box[4].points[i].p[0];
			Box[5].points[i].p[1] = y + d / 2;
			Box[5].points[i].p[2] = Box[4].points[i].p[2];
			Box[5].points[i].p[3] = Box[4].points[i].p[3];
			Box[5].points[i].p[4] = Box[4].points[i].p[4];
		}
	}
};