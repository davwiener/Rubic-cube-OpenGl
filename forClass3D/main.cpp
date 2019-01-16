#include <Windows.h>
#include <iostream>
#include "display.h"
#include "mesh.h"
#include "shader.h"
#include "inputManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


const int whichCub = 3;			// 2 / 3 / 4 / 5
using namespace glm;

static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 800;
float direction = 1.0;
float PI = 3.14159265358979323846;
float theta = radians(PI / 2);
std::vector<char> moves;
std::vector<char> reversMoves;
int numOfMoves = 0;
float saveTheta = theta, lastTheta = theta;
float mulAng = 3283;
int whatWall = whichCub * whichCub;
int whatAll = whichCub * whichCub * whichCub;
mat4 Matrixes[whichCub * whichCub * whichCub];
mat4 movesMatrixes[whichCub * whichCub * whichCub];
vec3 v[5 * 5 * 5];
int bottom[whichCub * whichCub];  //= { 10,11,12,13,14,15,16,17,18 };
int top[whichCub * whichCub];  // { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
int front[whichCub * whichCub];  // = { 0,1,2,5,10,11,14,19,22 };
int back[whichCub * whichCub];  // = { 4,6,8,13,15,17,18,16,17 };		// to check again. { maybe not- [ 5,3,2,1,0,9,10,11,12,14,19 ] }
int left[whichCub * whichCub];  // = { 5,6,9,10,15,18,22,23,26 };
int right[whichCub * whichCub];  // = { 2,4,8,11,13,17,19,21,25 };

								 // more walls for 5x5//4*4
int right2[whichCub * whichCub];
int left2[whichCub * whichCub];
int top2[whichCub * whichCub];
int bottom2[whichCub * whichCub];
int front2[whichCub * whichCub];
int back2[whichCub * whichCub];

void mulAngle(int kind) {

	
	if (theta <= abs(radians(PI/2)))
	{ 
		theta = theta * 2;
		if (kind == 0) {
			moves.push_back('A');
		}
	}
}

void divAngle(int kind) {
	theta = theta / 2;
	if (kind == 0) {
		moves.push_back('Z');
	}
}
void copyMat() {

	for (int i = 0; i < whichCub*whichCub*whichCub; i++) {
		Matrixes[i] = movesMatrixes[i];
	}
}
void rotateCube(int mtx1, float dir, char axis,bool arrowKey) {
	// dir = direction is 1 or -1, axis = x/y/z, mtx123 = idx of 3 little cubes
	
	

	mat4 rotxf = glm::rotate(mat4(1), theta*dir * mulAng, vec3(1, 0, 0));
	mat4 rotyf = glm::rotate(mat4(1), theta*dir * mulAng, vec3(0, 1, 0));
	mat4 rotzf = glm::rotate(mat4(1), theta*dir * mulAng, vec3(0, 0, 1));

	

	mat4 trans = glm::translate(mat4(1), v[20]);
	mat4 transop = glm::translate(mat4(1), -v[20]);

	mat4  T = trans * rotzf * transop;
	mat4 T1 = trans * rotxf * transop;
	mat4 T2 = trans * rotyf * transop;

	switch (axis) {
	case 'x': {
		if (arrowKey)
			Matrixes[mtx1] = T1 * Matrixes[mtx1];
		else
			movesMatrixes[mtx1] = T1 * movesMatrixes[mtx1];
	} break;

	case 'y': {
		if (arrowKey)
			Matrixes[mtx1] = T2 * Matrixes[mtx1];
		else
			movesMatrixes[mtx1] = T2 * movesMatrixes[mtx1];
	} break;

	case 'z': {
		if (arrowKey)
			Matrixes[mtx1] = T * Matrixes[mtx1];
		else
			movesMatrixes[mtx1] = T * movesMatrixes[mtx1];
	} break;
	};


}


void rotateCubeRight(int kind) {
	for (int i = 0; i < whatAll; i++) {
		rotateCube(i, 1.0, 'x',true);
	}
	if (kind == 0) {
		moves.push_back('R');
	}


}

void rotateCubeLeft(int kind) {
	for (int i = 0; i < whatAll; i++) {
		rotateCube(i, -1.0, 'x',true);
	}
	if (kind == 0) {
		moves.push_back('L');
	}


}

void rotateCubeUp(int kind) {
	for (int i = 0; i < whatAll; i++) {
		rotateCube(i, -1.0, 'y',true);
	}
	if (kind == 0) {
		moves.push_back('U');
	}


}
void rotateCubeDown(int kind) {
	for (int i = 0; i < whatAll; i++) {
		rotateCube(i, 1.0, 'y',true);
	}
	if (kind == 0) {
		moves.push_back('D');
	}


}



// 4 walls more of 5x5 cubes






void Domoves() {
	std::vector<char> doMove = moves;
	for (std::vector<char>::iterator it = moves.begin(); it != moves.end(); ++it)
	{
		char move = *it;
		if (move == 'A')
			mulAngle(1);
		else if (move == 'Z')
			divAngle(1);
		else if (move == 'U')
			rotateCubeUp(1);
		else if (move == 'D')
			rotateCubeDown(1);
		else if (move == 'R')
			rotateCubeRight(1);
		else if (move == 'L')
			rotateCubeLeft(1);

	}

}



void updateLocations(int numCubes) {		// numCubes = number of cubes
	int frontIndx = 0, backIndx = 0, bottomIndx = 0, topIndx = 0, leftIndx = 0, rightIndx = 0;
	int bottom2Indx = 0, top2Indx = 0, left2Indx = 0, right2Indx = 0, front2Indx = 0, back2Indx = 0;		// 4 more walls

	for (int i = 0; i < numCubes; i++) {

		mat4 tempM = movesMatrixes[i];
		float x = tempM[3].x;
		float y = tempM[3].y;
		float z = tempM[3].z;

		//	NOW changes for 2x2 CUBE !


		if (whichCub == 5) {	// 5x5

			if (x >= 1.02 * 4 && x <= 1.03 * 4) {
				top[topIndx] = i;
				topIndx++;
			}
			else if (x <= -1.02 * 4 && x >= -1.03 * 4) {
				bottom[bottomIndx] = i;
				bottomIndx++;
			}
			if (y >= 1.02 * 4 && y <= 1.03 * 4) {
				right[rightIndx] = i;
				rightIndx++;
			}
			else if (y <= -1.02 * 4 && y >= -1.03 * 4) {
				left[leftIndx] = i;
				leftIndx++;
			}
			if (z <= -1.02 * 2 && z >= -1.03 * 2) {
				front[frontIndx] = i;
				frontIndx++;
			}
			else if (z >= 1.02 * 6 && z <= 1.03 * 6) {
				back[backIndx] = i;
				backIndx++;
			}


			// insert 4 more walls to rotate


			if (x >= 1.02 * 2 && x <= 1.03 * 2) {
				top2[top2Indx] = i;
				top2Indx++;
			}
			else if (x <= -1.02 * 2 && x >= -1.03 * 2) {
				bottom2[bottom2Indx] = i;
				bottom2Indx++;
			}
			if (y >= 1.02 * 2 && y <= 1.03 * 2) {
				right2[right2Indx] = i;
				right2Indx++;
			}
			if (y <= -1.02 * 2 && y >= -1.03 * 2) {
				left2[left2Indx] = i;
				left2Indx++;
			}


			if (z <= 1.02 * 0.1 && z >= -1.03 * 0.1) {
				front2[front2Indx] = i;
				front2Indx++;
			}

			if (z >= 1.02 * 4 && z <= 1.03 * 4) {
				back2[back2Indx] = i;
				back2Indx++;
			}




		}


		else if (whichCub == 4) {	// 4x4

			if (x >= 1.02 * 3 && x <= 1.03 * 3) {
				top[topIndx] = i;
				topIndx++;
			}
			else if (x <= -1.02 * 3 && x >= -1.03 * 3) {
				bottom[bottomIndx] = i;
				bottomIndx++;
			}
			if (y >= 1.02 * 3 && y <= 1.03 * 3) {
				right[rightIndx] = i;
				rightIndx++;
			}
			else if (y <= -1.02 * 3 && y >= -1.03 * 3) {
				left[leftIndx] = i;
				leftIndx++;
			}
			if (z <= -1.02 * 3 && z >= -1.03 * 3) {
				front[frontIndx] = i;
				frontIndx++;
			}
			else if (z >= 1.02 * 3 && z <= 1.03 * 3) {
				back[backIndx] = i;
				backIndx++;
			}

			// more rotates for 4x4

			if (x >= 1.02 * 1 && x <= 1.03 * 1) {
				top2[top2Indx] = i;
				top2Indx++;
			}
			else if (x <= -1.02 * 1 && x >= -1.03 * 1) {
				bottom2[bottom2Indx] = i;
				bottom2Indx++;
			}
			if (y >= 1.02 * 1 && y <= 1.03 * 1) {
				right2[right2Indx] = i;
				right2Indx++;
			}
			if (y <= -1.02 * 1 && y >= -1.03 * 1) {
				left2[left2Indx] = i;
				left2Indx++;
			}


			if (z <= 1.02 * 1 && z >= -1.03 * 1) {
				front2[front2Indx] = i;
				front2Indx++;
			}

			if (z >= 1.02 * 1 && z <= 1.03 * 1) {
				back2[back2Indx] = i;
				back2Indx++;
			}


		}

		else if (whichCub == 2) {		// 2x2 cube

			if (x >= 1.02  && x <= 1.03) {
				top[topIndx] = i;
				topIndx++;
			}
			else if (x <= -1.02  && x >= -1.03) {
				bottom[bottomIndx] = i;
				bottomIndx++;
			}
			if (y >= 1.02 && y <= 1.03) {
				right[rightIndx] = i;
				rightIndx++;
			}
			else if (y <= -1.02 && y >= -1.03) {
				left[leftIndx] = i;
				leftIndx++;
			}
			if (z <= -1.02 && z >= -1.03) {
				front[frontIndx] = i;
				frontIndx++;
			}
			else if (z >= 1.02 && z <= 1.03) {
				back[backIndx] = i;
				backIndx++;
			}
		}

		else if (whichCub == 3) {		// 3x3 cube
			if (x >= 1.02 * 2 && x <= 1.03 * 2) {
				top[topIndx] = i;
				topIndx++;
			}
			if (x <= -1.02 * 2 && x >= -1.03 * 2) {
				bottom[bottomIndx] = i;
				bottomIndx++;
			}
			if (y >= 1.02 * 2 && y <= 1.03 * 2) {
				right[rightIndx] = i;
				rightIndx++;
			}
			if (y <= -1.02 * 2 && y >= -1.03 * 2) {
				left[leftIndx] = i;
				leftIndx++;
			}
			if (z <= 0.2 && z >= -0.2) {
				front[frontIndx] = i;
				frontIndx++;
			}
			if (z >= 1.02 * 4 && z <= 1.03 * 4) {			// changed
				back[backIndx] = i;
				backIndx++;
			}
		}
	}

}


void rotateWall(int wall[], char arrow) {
	updateLocations(whatAll);
	for (int i = 0; i < whatWall; i++) {
		rotateCube(wall[i], direction, arrow,false);
	}
	theta = radians(PI/2);
	copyMat();
	Domoves();
}


void key_callback2(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);

		case GLFW_KEY_R: {

			if (action == GLFW_REPEAT || action == GLFW_PRESS)
				rotateWall(right, 'y');

			break;
		}
		case GLFW_KEY_L: {
			if (action == GLFW_REPEAT || action == GLFW_PRESS)
				rotateWall(left, 'y');
			break;
		}
		case GLFW_KEY_U: {
			if (action == GLFW_REPEAT || action == GLFW_PRESS)
				rotateWall(top, 'x');
			break;
		}
		case GLFW_KEY_D: {
			if (action == GLFW_REPEAT || action == GLFW_PRESS)
				rotateWall(bottom, 'x');
			break;
		}

						 // 4 more rotations to 5x5 cube

		case GLFW_KEY_E: {

			if (action == (GLFW_REPEAT || action == GLFW_PRESS) && whichCub>3)
				rotateWall(right2, 'y');

			break;
		}
		case GLFW_KEY_K: {
			if ((action == GLFW_REPEAT || action == GLFW_PRESS) && whichCub>3)
				rotateWall(left2, 'y');
			break;
		}
		case GLFW_KEY_J: {
			if ((action == GLFW_REPEAT || action == GLFW_PRESS) && whichCub>3)
				rotateWall(top2, 'x');
			break;
		}
		case GLFW_KEY_C: {
			if ((action == GLFW_REPEAT || action == GLFW_PRESS) && whichCub>3)
				rotateWall(bottom2, 'x');
			break;
		}
		case GLFW_KEY_N: {
			if ((action == GLFW_REPEAT || action == GLFW_PRESS) && whichCub>3)
				rotateWall(back2, 'z');
			break;
		}

		case GLFW_KEY_G: {
			if ((action == GLFW_REPEAT || action == GLFW_PRESS) && whichCub>3)
				rotateWall(front2, 'z');
			break;
		}

		case GLFW_KEY_B: {
			if (action == GLFW_REPEAT || action == GLFW_PRESS)
				rotateWall(back, 'z');
			break;
		}
		case GLFW_KEY_F: {
			if (action == GLFW_REPEAT || action == GLFW_PRESS)
				rotateWall(front, 'z');
			break;
		}
		case GLFW_KEY_SPACE: {
			if (action == GLFW_PRESS)
				direction = -direction;
			break;
		}

		case GLFW_KEY_Z: {
			if (action == GLFW_PRESS)
				divAngle(0);
			break;
		}
		case GLFW_KEY_A: {
			if (action == GLFW_PRESS)
				mulAngle(0);
			break;
		}
		case GLFW_KEY_RIGHT: {
			if (action == GLFW_REPEAT || action == GLFW_PRESS)
				rotateCubeRight(0);
			break;
		}
		case GLFW_KEY_LEFT: {
			if (action == GLFW_REPEAT || action == GLFW_PRESS)
				rotateCubeLeft(0);
			break;
		}
		case GLFW_KEY_UP: {
			if (action == GLFW_REPEAT || action == GLFW_PRESS)
				rotateCubeUp(0);
			break;
		}
		case GLFW_KEY_DOWN: {
			if (action == GLFW_REPEAT || action == GLFW_PRESS)
				rotateCubeDown(0);
			break;
		}
		default:
			break;
		}
	}

}



void initMat5() {


	whatWall = whichCub * whichCub;
	whatAll = whichCub * whichCub * whichCub;
	float K = 1.025 * 2;

	for (int iX = 0; iX < whichCub; iX++) {

		for (int iY = 0; iY < whichCub; iY++) {

			for (int iZ = 0; iZ < whichCub; iZ++) {

				v[iZ + iY*whichCub + iX*whatWall] = vec3(float(K*iX - K * 2), float(K*iY - K * 2), float(iZ*K - K));
				Matrixes[iZ + iY*whichCub + iX*whatWall] = glm::translate(mat4(1), v[iZ + iY*whichCub + iX*whatWall]);  //front top left
				movesMatrixes[iZ + iY*whichCub + iX*whatWall] = glm::translate(mat4(1), v[iZ + iY*whichCub + iX*whatWall]);
			}

		}

	}


	v[20] = vec3(0, 0, 2.05); // center of rotation


}


void initMat4() {


	whatWall = whichCub * whichCub;
	whatAll = whichCub * whichCub * whichCub;
	float K = 1.025 * 2;

	for (int iX = 0; iX < whichCub; iX++) {

		for (int iY = 0; iY < whichCub; iY++) {

			for (int iZ = 0; iZ < whichCub; iZ++) {

				v[iZ + iY*whichCub + iX*whatWall] = vec3(float(K*iX - 1.5*K), float(K*iY - 1.5*K), float(iZ*K - 1.5*K));
				Matrixes[iZ + iY*whichCub + iX*whatWall] = glm::translate(mat4(1), v[iZ + iY*whichCub + iX*whatWall]);  //front top left
				movesMatrixes[iZ + iY*whichCub + iX*whatWall] = glm::translate(mat4(1), v[iZ + iY*whichCub + iX*whatWall]);
			}

		}

	}


	v[20] = vec3(0, 0, 0); // center of rotation


}



void initMat2() {

	v[0] = vec3(1.025, -1.025, -1.025);
	Matrixes[0] = glm::translate(mat4(1), v[0]);  //front top left
	movesMatrixes[0] = glm::translate(mat4(1), v[0]);  //front top left

	v[1] = vec3(1.025, 1.025, -1.025);
	Matrixes[1] = glm::translate(mat4(1), v[1]);  //front top right
	movesMatrixes[1] = glm::translate(mat4(1), v[1]);  //front top right

	v[2] = vec3(-1.025, -1.025, -1.025);
	Matrixes[2] = glm::translate(mat4(1), v[2]);  //front middile left
	movesMatrixes[2] = glm::translate(mat4(1), v[2]);  //front middile left

	v[3] = vec3(-1.025, 1.025, -1.025);
	Matrixes[3] = glm::translate(mat4(1), v[3]);  //front middile right
	movesMatrixes[3] = glm::translate(mat4(1), v[3]);  //front middile right


	v[4] = vec3(1.025, -1.025, 1.025);
	Matrixes[4] = glm::translate(mat4(1), v[4]);  //secuned line top left  
	movesMatrixes[4] = glm::translate(mat4(1), v[4]);  //secuned line top left  

	v[5] = vec3(1.025, 1.025, 1.025);
	Matrixes[5] = glm::translate(mat4(1), v[5]);  //sec. line top right
	movesMatrixes[5] = glm::translate(mat4(1), v[5]);  //sec. line top right

	v[6] = vec3(-1.025, -1.025, 1.025);
	Matrixes[6] = glm::translate(mat4(1), v[6]);  //secuned line middile left
	movesMatrixes[6] = glm::translate(mat4(1), v[6]);  //secuned line middile left

	v[7] = vec3(-1.025, 1.025, 1.025);
	Matrixes[7] = glm::translate(mat4(1), v[7]);  //secuned line middile right
	movesMatrixes[7] = glm::translate(mat4(1), v[7]);  //secuned line middile right

	v[20] = vec3(0, 0, 0); // center of rotation


}

void initMat() {

	v[0] = vec3(1, 1, 1);
	movesMatrixes[0] = glm::mat4(1);  //front middile center  -----center rotation -----

	v[1] = vec3(2.05, 0, 0);
	movesMatrixes[1] = glm::translate(mat4(1), v[1]);  //front top center

	v[2] = vec3(2.05, 2.05, 0);
	movesMatrixes[2] = glm::translate(mat4(1), v[2]);  //front top right

	v[3] = vec3(2.05, 0, 2.05);
	movesMatrixes[3] = glm::translate(mat4(1), v[3]);  //secuned line top center

	v[4] = vec3(2.05, 2.05, 2.05);
	movesMatrixes[4] = glm::translate(mat4(1), v[4]);  //secuned line top right

	v[5] = vec3(2.05, -2.05, 0);
	movesMatrixes[5] = glm::translate(mat4(1), v[5]);  //front top left

	v[6] = vec3(2.05, -2.05, 2.05);
	movesMatrixes[6] = glm::translate(mat4(1), v[6]);  //secuned line top left  

	v[7] = vec3(2.05, 0, 4.1);
	movesMatrixes[7] = glm::translate(mat4(1), v[7]);  //back top center

	v[8] = vec3(2.05, 2.05, 4.1);
	movesMatrixes[8] = glm::translate(mat4(1), v[8]);  //back top right

	v[9] = vec3(2.05, -2.05, 4.1);
	movesMatrixes[9] = glm::translate(mat4(1), v[9]);  //back top left

	v[10] = vec3(-2.05, -2.05, 0);
	movesMatrixes[10] = glm::translate(mat4(1), v[10]);  // front bottom

	v[11] = vec3(-2.05, 2.05, 0);
	movesMatrixes[11] = glm::translate(mat4(1), v[11]);  //front bottom right

	v[12] = vec3(-2.05, 0, 2.05);
	movesMatrixes[12] = glm::translate(mat4(1), v[12]);  //secuned line bottom cernter

	v[13] = vec3(-2.05, 2.05, 2.05);
	movesMatrixes[13] = glm::translate(mat4(1), v[13]);  //secuned line bottom right

	v[14] = vec3(-2.05, 0, 0);
	movesMatrixes[14] = glm::translate(mat4(1), v[14]);  // center bottom front

	v[15] = vec3(-2.05, -2.05, 2.05);
	movesMatrixes[15] = glm::translate(mat4(1), v[15]);  //secuned line bottom left

	v[16] = vec3(-2.05, 0, 4.1);
	movesMatrixes[16] = glm::translate(mat4(1), v[16]);  // back bottom center

	v[17] = vec3(-2.05, 2.05, 4.1);
	movesMatrixes[17] = glm::translate(mat4(1), v[17]);  //back bottom right

	v[18] = vec3(-2.05, -2.05, 4.1);
	movesMatrixes[18] = glm::translate(mat4(1), v[18]);  //back bottom left

	v[19] = vec3(0, 2.05, 0);
	movesMatrixes[19] = glm::translate(mat4(1), v[19]);  //front middile right

	v[20] = vec3(0, 0, 2.05);
	movesMatrixes[20] = glm::translate(mat4(1), v[20]);  //secuned line middile center-----center rotation

	v[21] = vec3(0, 2.05, 2.05);
	movesMatrixes[21] = glm::translate(mat4(1), v[21]);  //secuned line middile right

	v[22] = vec3(0, -2.05, 0);
	movesMatrixes[22] = glm::translate(mat4(1), v[22]);  //front middile left

	v[23] = vec3(0, -2.05, 2.05);
	movesMatrixes[23] = glm::translate(mat4(1), v[23]);  //secuned line middile left

	v[24] = vec3(0, 0, 4.1);
	movesMatrixes[24] = glm::translate(mat4(1), v[24]);  //back middile center

	v[25] = vec3(0, 2.05, 4.1);
	movesMatrixes[25] = glm::translate(mat4(1), v[25]);  //back middile right

	v[26] = vec3(0, -2.05, 4.1);
	movesMatrixes[26] = glm::translate(mat4(1), v[26]);  //back middile left

	copyMat();

}


int main(int argc, char** argv)
{
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");

	Vertex vertices[] =
	{
		Vertex(glm::vec3(-1, -1, -1), glm::vec2(1, 0), glm::vec3(0, 0, -1),glm::vec3(0, 0, 1)),  /* blue wall */
		Vertex(glm::vec3(-1, 1, -1), glm::vec2(0, 0), glm::vec3(0, 0, -1),glm::vec3(0, 0, 1)),
		Vertex(glm::vec3(1, 1, -1), glm::vec2(0, 1), glm::vec3(0, 0, -1),glm::vec3(0, 0, 1)),
		Vertex(glm::vec3(1, -1, -1), glm::vec2(1, 1), glm::vec3(0, 0, -1),glm::vec3(0, 0, 1)),

		Vertex(glm::vec3(-1, -1, 1), glm::vec2(1, 0), glm::vec3(0, 0, 1),glm::vec3(0, 1, 1)),  /* g&b wall */
		Vertex(glm::vec3(-1, 1, 1), glm::vec2(0, 0), glm::vec3(0, 0, 1),glm::vec3(0, 1, 1)),
		Vertex(glm::vec3(1, 1, 1), glm::vec2(0, 1), glm::vec3(0, 0, 1),glm::vec3(0, 1, 1)),
		Vertex(glm::vec3(1, -1, 1), glm::vec2(1, 1), glm::vec3(0, 0, 1),glm::vec3(0, 1, 1)),

		Vertex(glm::vec3(-1, -1, -1), glm::vec2(0, 1), glm::vec3(0, -1, 0),glm::vec3(1, 1, 0)),  /* r&g wall */
		Vertex(glm::vec3(-1, -1, 1), glm::vec2(1, 1), glm::vec3(0, -1, 0),glm::vec3(1, 1, 0)),
		Vertex(glm::vec3(1, -1, 1), glm::vec2(1, 0), glm::vec3(0, -1, 0),glm::vec3(1, 1, 0)),
		Vertex(glm::vec3(1, -1, -1), glm::vec2(0, 0), glm::vec3(0, -1, 0),glm::vec3(1, 1, 0)),

		Vertex(glm::vec3(-1, 1, -1), glm::vec2(0, 1), glm::vec3(0, 1, 0),glm::vec3(0, 1, 0)),  /* green wall */
		Vertex(glm::vec3(-1, 1, 1), glm::vec2(1, 1), glm::vec3(0, 1, 0),glm::vec3(0, 1, 0)),
		Vertex(glm::vec3(1, 1, 1), glm::vec2(1, 0), glm::vec3(0, 1, 0),glm::vec3(0, 1, 0)),
		Vertex(glm::vec3(1, 1, -1), glm::vec2(0, 0), glm::vec3(0, 1, 0),glm::vec3(0, 1, 0)),

		Vertex(glm::vec3(-1, -1, -1), glm::vec2(1, 1), glm::vec3(-1, 0, 0),glm::vec3(1, 0, 0)),  /* red wall */
		Vertex(glm::vec3(-1, -1, 1), glm::vec2(1, 0), glm::vec3(-1, 0, 0),glm::vec3(1, 0, 0)),
		Vertex(glm::vec3(-1, 1, 1), glm::vec2(0, 0), glm::vec3(-1, 0, 0),glm::vec3(1, 0, 0)),
		Vertex(glm::vec3(-1, 1, -1), glm::vec2(0, 1), glm::vec3(-1, 0, 0),glm::vec3(1, 0, 0)),

		Vertex(glm::vec3(1, -1, -1), glm::vec2(1, 1), glm::vec3(1, 0, 0),glm::vec3(1, 0, 1)),  /* r&b wall */
		Vertex(glm::vec3(1, -1, 1), glm::vec2(1, 0), glm::vec3(1, 0, 0),glm::vec3(1, 0, 1)),
		Vertex(glm::vec3(1, 1, 1), glm::vec2(0, 0), glm::vec3(1, 0, 0),glm::vec3(1, 0, 1)),
		Vertex(glm::vec3(1, 1, -1), glm::vec2(0, 1), glm::vec3(1, 0, 0),glm::vec3(1, 0, 1))
	};
	unsigned int indices[] = { 0, 1, 2,
		0, 2, 3,

		6, 5, 4,
		7, 6, 4,

		10, 9, 8,
		11, 10, 8,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		22, 21, 20,
		23, 22, 20
	};

	Mesh mesh(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0]));
	//Mesh monkey("./res/meshes/monkeyNoUV.obj");
	Shader shader("./res/shaders/basicShader");
	if (whichCub == 3)
		initMat();		// initialize mat's 2 / 3 / 4 / 5
	else if (whichCub == 2)
		initMat2();		// initialize mat's 2 / 3 / 4 / 5
	else if (whichCub == 4)
		initMat4();		// initialize mat's 2 / 3 / 4 / 5
	else if (whichCub == 5)
		initMat5();		// initialize mat's 2 / 3 / 4 / 5

	int lview = -7;

	if (whichCub > 3)
		lview = -10;

	vec3 pos = vec3(0, 0, lview);
	vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
	vec3 up = glm::vec3(1.0f, 0.0f, 0.0f);
	vec3 side = glm::vec3(0.0f, 1.0f, 0.0f);
	mat4 P = glm::perspective(100.0f, (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, 0.1f, 100.0f);



	mat4 M = glm::mat4(1);
	mat4 M1 = glm::translate(mat4(1), vec3(3, 0, 0));
	mat4 M2 = glm::translate(mat4(1), vec3(-3, 0, 0));
	P = P * glm::lookAt(pos, pos + forward, up);
	mat4 MVP = P*M;

	updateLocations(whatAll);			// update little cubes location (idx's)

	glfwSetKeyCallback(display.m_window, key_callback2);	// every key cause to call back
	while (!glfwWindowShouldClose(display.m_window))		// ** need to check why is 2 sessions of windows and why the first is go out **
	{
		Sleep(1);
		display.Clear(1.0f, 1.0f, 1.0f, 1.0f);
		for (int i = 0; i < whatAll; i++) {
			MVP = P*Matrixes[i];
			shader.Bind();
			shader.Update(MVP,Matrixes[i]);
			mesh.Draw();
		}

		display.SwapBuffers();
		glfwPollEvents();
	}
	return 0;
}
