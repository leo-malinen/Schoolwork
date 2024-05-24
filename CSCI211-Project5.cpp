#include <iostream>
#include <vector>
using namespace std;
#include "shape.h"
#include "triangle.h"
#include "circle.h"
#include "square.h"
#include "point.h"
#include "grid.h"

int main()
{
    string shape;
    int x, y;
    int size;
    char letter;

    vector<Shape*> shapes;

    while (cin >> shape)
    {
        if (shape == "square")
        {
            cin >> x;
              cin >> y;
            cin >> size;
            shapes.push_back(new Square(x, y, size));
        }
        if (shape == "triangle")
        {
            cin >> x;
            cin >> y;
            shapes.push_back(new Triangle(x, y));
        }
        if (shape == "circle")
        {
            cin >> x;
            cin >> y;
            shapes.push_back(new Circle(x, y));
        }
        if (shape == "point")
        {
            cin >> x;
            cin >> y;
            cin >> letter;
            shapes.push_back(new Point(x, y, letter));
        }
    }

    Grid grid;

    for (unsigned int i = 0; i < shapes.size(); i++)
    {
      shapes[i]->draw(grid);
    }

    grid.print();

    return 0;
}
