#include "sand.h"

bool checkCell(int cellX, int cellY)
{
	if ((cellX < 0) || (cellX >= map->sizeX) || (cellY < 0) || (cellY >= map->sizeY))
		return true;

	return false;
}

bool fix_cell(int& cellX, int& cellY)
{
	bool error = false;

	if (cellX < 0)
	{
		cellX = 0;
		error = true;
	}
	else if (cellX >= map->sizeX)
	{
		cellX = map->sizeX - 1;
		error = true;
	}
	if (cellY < 0)
	{
		cellY = 0;
		error = true;
	}
	else if (cellY >= map->sizeY)
	{
		cellY = map->sizeY - 1;
		error = true;
	}

	return error;
}
/*
int min(int num1, int num2)
{
	int minimum = num1;

	if (num2 < num1)
		minimum = num2;

	return minimum;
}

int max(int num1, int num2)
{
	int maximum = num2;

	if (num2 < num1)
		maximum = num1;

	return maximum;
}
*/
int power(int num1, int num2)
{
	int result = 1;
	while (num2-- > 0)
		result *= num1;

	return result;
}

bool testIfInt(double number)
{
	bool result;

	double temp = number - (int)number;
	if (temp <= 0.0001 && temp >= -0.9999)
		result = true;
	else
		result = false;

	return result;
}
double fixDouble(double number)
{
	if (fabs(number - (double)((int)number)) < 0.00001)
		number = (double)((int)number);

	return number;
}
int roundUp(double number)
{
	int result = (int)number;

	if (!testIfInt(number)) result++;

	return result;
}

double dest_angle(COORDTYPE* p1, COORDTYPE* p2)
{
        double  destAngle;

        double  diffX = p2->x - p1->x,
                diffY = p2->y - p1->y;

        if (diffX != 0)
        {
                if ((diffX >= 0) && (diffY >= 0))  //if x2 >= x1 && y2 >= y1
                {
                        destAngle = 2*PI - atan(fabs(diffY/diffX));
                }
                else if ((diffX < 0) && (diffY >= 0))  //if x2 < x1 && y2 >= y1
                {
                        destAngle = atan(fabs(diffY/diffX)) + PI;
                }
                else if ((diffX < 0) && (diffY < 0))  //if x2 < x1 && y2 < y1
                {
                        destAngle = PI - atan(fabs(diffY/diffX));
                }
                else if ((diffX >= 0) && (diffY < 0))  //if x2 >= x1 && y2 < y1
                {
                        destAngle = atan(fabs(diffY/diffX));
                }
        }
        else if (diffY <= 0)
        {
                destAngle = PI/2;
        }
        else destAngle = 3*PI/2;

        if (destAngle == 2*PI) destAngle = 0;

        return (destAngle*256.0/(2*PI));
}

int getRandomInt(int min, int max)
{
	max++;
	return ((rand() % (max-min)) + min);
}
double normalize(double value, double max)
{
	while(value < 0.0)
		value += max;
	while(value > max)
		value -= max;

	return(value);
}

double min_delta(double from, double to, double max)
{
	double d1 = from - to;
	double d2 = (from + max) - to;
	double d3 = from - (to + max);
	double val = d1;

	if(fabs(val) > fabs(d2)) val = d2;
	if(fabs(val) > fabs(d3)) val = d3;
	return(val);
}
/*
double trajectory_angle(double x1, double y1, double x2, double y2)
{ 
  double dx = min_delta(x1, x2, X_MAX);
  double dy = min_delta(y1, y2, Y_MAX);
  double alpha = atan(dy / dx);

  if(dx < 0.0)
    alpha += PI;
  alpha = normalize(alpha, PI * 2.0);

  alpha = ((alpha + PI) / (PI * 2.0)) * 256.0;
  alpha = normalize(alpha, 256.0);
 return alpha;
}
*/
/*
double wcorner(double x, double w)
{
  double value = (x - (w / 2.0) - space_x);

  while(value < (-w))
    value += X_MAX;
  while(value > (X_MAX - w))
    value -= X_MAX;
  value *= space_zoom;

  return(value);
}

double hcorner(double y, double h)
{
  double value = (y - (h / 2.0) - space_y);

  while(value < (-h))
    value += Y_MAX;
  while(value > (Y_MAX - h))
    value -= Y_MAX;
  value *= space_zoom;

  return(value);
}

double nearest_coord(double from, double to, double max)
{
  return(from - min_delta(from, to, max));
}
*/
double distance_from(COORDTYPE* p1, COORDTYPE* p2)
{
	double	first = (p1->x - p2->x),
			second = (p1->y - p2->y);

	return(sqrt(first*first + second*second));
}
double distance_from(double x, double y, double to_x, double to_y)
{
	double	first = (x - to_x),
			second = (y - to_y);

	return(sqrt(first*first + second*second));
}
double hypotinuse_of(double x, double y)
{
	return(sqrt((x * x) + (y * y)));
}
double blockDistance(COORDTYPE* p1, COORDTYPE* p2)
{
	int xDis = abs(p1->x - p2->x),
		yDis = abs(p1->y - p2->y),
		minDis = std::min(xDis, yDis);

	return ((double)std::max(xDis, yDis) + (double)minDis*(DIAGONALCOST - 1.0));
	//return (((double)minDis)*DIAGONALCOST + max(xDis, yDis) - minDis);
}

double closestBlockDistance(COORDTYPE* objectLocation, COORDTYPE* targetLocation, COORDTYPE* size)
{
	COORDTYPE closestPoint;

	//find the closest cell of a structure from a location
	if (objectLocation->x <= targetLocation->x)	//if we are left of the structure
		closestPoint.x = targetLocation->x;	//set destination, left most point
	else if (objectLocation->x >= (targetLocation->x + size->x-1))	//vica versa
		closestPoint.x = targetLocation->x + size->x-1;
	else
		closestPoint.x = objectLocation->x;	//we are above or below at least on cell of the structure, closest path is straight

	//same deal but with y
	if (objectLocation->y <= targetLocation->y)
		closestPoint.y = targetLocation->y;
	else if (objectLocation->y >= (targetLocation->y + size->y-1))
		closestPoint.y = targetLocation->y + size->y-1;
	else
		closestPoint.y = objectLocation->y;

	return blockDistance(objectLocation, &closestPoint);
}


void split(char value[][MAX_LINE], char* line, char seperator)
{
	int i = 0,
		j = 0,
		current = 0,
		length = strlen(line);

	while (i < length)
	{
		if ((line[i] != seperator) && ((line[i] != '\n') && (line[i] != '\r')))
			value[current][j++] = line[i];
		else
		{
			value[current++][j] = '\0';
			j = 0;
		}
		i++;
	}
	value[current][j] = '\0';
}

int toInt(char* line)
{
	int	i = 0,
		length = 0,
		result = 0;

	length = strlen(line);

	i = 0;
	while (length > 0)
		result += ((int)line[i++] - 48) * power(10, --length);

	return result;
}

