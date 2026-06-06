#include "App.h"

int main()
{
	App App(1000, 800, "DiceRoll");
	App.Run();
}

/*
* TODO:
* 
* - make a better solution for collisionsolver's transformwriting
* - in support mapping its better practice to solve for local space support mapping, then transform it into world space
* - when checking entity - plane collision for polyhedra consisting of more complex faces than a triangle its better
	to calculate avarage of all penetrating points
*/