// Project #2 : Draw a Maze
//
// Class : Graphics - Simposon
// Name	 : Payton Fisher
// Date	 : 1-29-2019

#include "Angel.h"
#include "vector"
#pragma comment( lib, "glew32" )
const int mazeSize = 35; //Single variable for changing how many spirals to draw
struct cell {
	bool visited = false, right = false, down = false;
};
cell map[mazeSize + 2][mazeSize + 2];

void recur(int, int);

//----------------------------------------------------------------------------

void init(void) {

	vec2 points[mazeSize * mazeSize * 4 + 4 * mazeSize];
	float cellSize = 2.0 / (mazeSize + 2); //Size of a single cell
	float cellStep = cellSize / 2; //Distance away from center of cell
	int pointsCounter = 0; //Counter for loading points into points array

	//Generate grid for mazethe maze

	for (int i = 1; i <= mazeSize; i++) {
		for (int j = 1; j <= mazeSize; j++) {
			map[i][j].down = true;
			map[i][j].right = true;
		}
	}

	//Make the cuts in the maze

	recur(0, 0);

	//Set the top and left side borders

	for (int i = 1; i <= mazeSize; i++) {
		map[i][0].down = true;
		map[0][i].right = true;
	}

	//Pick a random point for start and random point for end of maze
	int start = rand() % mazeSize;
	int end = rand() % mazeSize;

	map[0][start + 1].right = false;
	map[mazeSize][end + 1].right = false;

	//Draw in all of the lines for the cells

	for (int i = 0; i <= mazeSize; i++) {
		for (int j = 0; j <= mazeSize; j++) {
			vec2 pos = vec2(-1 + cellSize * i, 1 - cellSize * j); //Calculates pos from top left corner (-1,1)
			if (map[i][j].down) {
				points[pointsCounter] = pos + vec2(0, -cellSize);
				pointsCounter++;
				points[pointsCounter] = pos + vec2(cellSize, -cellSize);
				pointsCounter++;
			}
			if (map[i][j].right) {
				points[pointsCounter] = pos + vec2(cellSize, 0);
				pointsCounter++;
				points[pointsCounter] = pos + vec2(cellSize, -cellSize);
				pointsCounter++;
			}
		}
	}

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader_a2.glsl", "fshader_a2.glsl");
	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}

//----------------------------------------------------------------------------

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);     // clear the window
	glDrawArrays(GL_LINES, 0, mazeSize * mazeSize * 4 + 4 * mazeSize);    // draw the points
	glFlush();
}

//----------------------------------------------------------------------------

void recur(int x, int y) {
	std::vector<vec2> possibleMoves;
	
	map[x][y].visited = true;

	//Check to see what moves are possible to perform
	if (y - 1 > 0 && !map[x][y - 1].visited) { //Up
		possibleMoves.push_back(vec2(0, -1));
	}
	if (x + 1 < mazeSize + 1 && !map[x + 1][y].visited) { //Right
		possibleMoves.push_back(vec2(1, 0));
	}
	if (x - 1 > 0 && !map[x - 1][y].visited) { //Left
		possibleMoves.push_back(vec2(-1, 0));
	}
	if (y + 1 < mazeSize + 1 && !map[x][y + 1].visited) { //Down
		possibleMoves.push_back(vec2(0, 1));
	}

	while (possibleMoves.size() > 0) {
		int randomPick = rand() % possibleMoves.size();
		bool completed = false;
		if (possibleMoves[randomPick].y == -1 && !map[x][y - 1].visited) { //Up
			map[x][y - 1].down = false;
			completed = true;
		}
		if (possibleMoves[randomPick].x == 1 && !map[x + 1][y].visited) { //Right
			map[x][y].right = false;
			completed = true;
		}
		if (possibleMoves[randomPick].x == -1 && !map[x - 1][y].visited) { //Left
			map[x - 1][y].right = false;
			completed = true;
		}
		if (possibleMoves[randomPick].y == 1 && !map[x][y + 1].visited) { //Down
			map[x][y].down = false;
			completed = true;
		}

		if(completed) //Check to see if completed in case along the search the previously possible move is not longer possible
			recur(x + possibleMoves[randomPick].x, y + possibleMoves[randomPick].y);
		possibleMoves.erase(possibleMoves.begin() + randomPick);
	}
}

//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033:
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(780, 780);
	// If you are using freeglut, the next two lines will check if 
	// the code is truly 3.2. Otherwise, comment them out
	glutInitContextVersion(3, 1);// Does not need glewExperimental = GL_TRUE
								 // 3,2 does.  Discovered by Dr. C
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Sierpinski Gasket");
	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}