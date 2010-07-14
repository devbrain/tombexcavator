bool checkCell(int cellX, int cellY);
bool fix_cell(int& cellX, int& cellY);
int power(int num1, int num2);
bool testIfInt(double number);
double fixDouble(double number);
int roundUp(double number);

#ifdef _MSC_VER 
inline int round(double number) { return ((int)(number + 0.5)); }
#endif

double dest_angle(COORDTYPE* p1, COORDTYPE* p2);
int getRandomInt(int min, int max);
double normalize(double value, double max);
double min_delta(double from, double to, double max);
double trajectory_angle(double x1, double y1, double x2, double y2);
double distance_from(COORDTYPE* p1, COORDTYPE* p2);
double distance_from(double x, double y, double to_x, double to_y);
double hypotinuse_of(double x, double y);
double blockDistance(COORDTYPE* p1, COORDTYPE* p2);
double closestBlockDistance(COORDTYPE* objectLocation, COORDTYPE* targetLocation, COORDTYPE* size);
void split(char value[][MAX_LINE+1], char* line, char seperator);
int toInt(char* line);

