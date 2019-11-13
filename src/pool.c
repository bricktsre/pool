/*
 * triangle.c
 *
 *  Created on: Aug 28, 2017
 *      Author: Thumrongsak Kosiyatrakul
 */


#ifdef __APPLE__  // include Mac OS X verions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#endif  // __APPLE__

#include "initShader.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrixOperations.h"
#include "geometricShapes.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

typedef GLfloat vec4[4];
typedef GLfloat mat4[16];
typedef GLfloat vec2[2];

int num_vertices = 0;
GLfloat angle = 0.0, phi = 0.0, step = 0.0;
GLuint model_view_location, projection_location, ctm_location;
mat4 model_view, projection, ctm;
int circle=1, move_down=0, forward=0, left=0, right=0;
int stage = 0, direction =3, new_direction = -1, sol_count = 0, last_f = 0;
vec4 old_a, old_e;
char solution[60];

vec4 cube_vertices[36] = {
	{0.0,1.0,0.0,1.0},{0.0,0.0,0.0,1.0},{1.0,0.0,0.0,1.0},
	{0.0,1.0,0.0,1.0},{1.0,0.0,0.0,1.0},{1.0,1.0,0.0,1.0},
	{1.0,1.0,0.0,1.0},{1.0,0.0,0.0,1.0},{1.0,0.0,-1.0,1.0},
	{1.0,1.0,0.0,1.0},{1.0,0.0,-1.0,1.0},{1.0,1.0,-1.0,1.0},
	{1.0,1.0,-1.0,1.0},{1.0,0.0,-1.0,1.0},{0.0,0.0,-1.0,1.0},
	{1.0,1.0,-1.0,1.0},{0.0,0.0,-1.0,1.0},{0.0,1.0,-1.0,1.0},
	{0.0,1.0,-1.0,1.0},{0.0,0.0,-1.0,1.0},{0.0,0.0,0.0,1.0},
	{0.0,1.0,-1.0,1.0},{0.0,0.0,0.0,1.0},{0.0,1.0,0.0,1.0},
	{0.0,1.0,0.0,1.0},{1.0,1.0,0.0,1.0},{1.0,1.0,-1.0,1.0},
	{0.0,1.0,0.0,1.0},{1.0,1.0,-1.0,1.0},{0.0,1.0,-1.0,1.0},
	{0.0,0.0,0.0,1.0},{0.0,0.0,-1.0,1.0},{1.0,0.0,-1.0,1.0},
	{0.0,0.0,0.0,1.0},{1.0,0.0,-1.0,1.0},{1.0,0.0,0.0,1.0},
};

void makeCube(vec4 *vertices, vec4 *normals, int *v_index, int *n_index){
	
}	

void fill(vec4 *vertices, vec2 *textures) {
	makeCube(vertices,textures,&v_index,&n_index);
}

void init(void)
{
	num_vertices = 5796;
	vec4 vertices[num_vertices];
	vec4 normals[num_vertices];
	fill(maze,vertices,textures);
	//vec4 colors[num_vertices];

	/*int width = 800;
	int height = 800;
	GLubyte my_texels[width][height][3];

	FILE *fp = fopen("p2texture04.raw", "r");
	fread(my_texels, width * height * 3, 1, fp);
	fclose(fp);
	*/
	GLuint program = initShader("vshader_ctm.glsl", "fshader.glsl");
	glUseProgram(program);

	/*GLuint mytex[1];
	glGenTextures(1, mytex);
	glBindTexture(GL_TEXTURE_2D, mytex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, my_texels);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	int param;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &param);
	*/
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+ sizeof(textures)/* + sizeof(colors)*/, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(textures), textures); 
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	/*GLuint vColor = glGetAttribLocation(program, "vColor");
	  glEnableVertexAttribArray(vColor);
	  glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));
	 
	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0 + (sizeof(vertices)/* + sizeof(colors)));

	GLuint texture_location = glGetUniformLocation(program, "texture");
	glUniform1i(texture_location, 0);
	*/
	identityMatrix(model_view);
	vec4 e = {0,5.0,4.0,0.0};
	vec4 a = {0.0,0.0,-4,0.0};
	vec4 vup = {0.0,1.0,0.0,0.0};
	lookAt(e,a,vup,model_view);
	//identityMatrix(projection);
	vec4 lrb = {-0.7,0.7,-0.7,0.0};
	vec4 tnf = {0.7,-1.0,-10,0.0};
	frustum(lrb,tnf,projection);
	identityMatrix(ctm);
	model_view_location = glGetUniformLocation(program, "model_view_matrix");
	projection_location = glGetUniformLocation(program, "projection_matrix");
	ctm_location = glGetUniformLocation(program, "ctm");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glDepthRange(1,0);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, ctm);
	glUniformMatrix4fv(model_view_location, 1, GL_FALSE, model_view);
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection);	

	glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
	if(key == 'q')
		exit(0);
	if(key == 'c'){
		GLfloat i,j,k;
		scanf("%f %f %f", &i,&j,&k);
		vec4 lrb = {i,j,k,0.0};
		scanf("%f %f %f", &i,&j,&k);
		vec4 tnf = {i,j,k,0.0};
		identityMatrix(projection);
		frustum(lrb,tnf,projection);
		glutPostRedisplay();
	}

	//glutPostRedisplay();
}

void idle(void) {
	if(circle){
		GLfloat x = -sin(angle) * 8;
		GLfloat z = -4 + cos(angle) * 8;
		vec4 e = {x,5.0,z,0.0};
		vec4 a = {0.0,0.0,-4,0.0};
		vec4 vup = {0.0,1.0,0.0,0.0};
		lookAt(e,a,vup,model_view);
		angle+=0.1;
		if(angle > M_PI*2){
			circle = 0;
			move_down = 1;
		}	
	} else if(move_down){
		if(stage == 0){
			GLfloat x = -sin(angle) * 8;
			GLfloat z = -4 + cos(angle) * 8;
			vec4 e = {x,5.0,z,0.0};
			vec4 a = {0.0,0.0,-4,0.0};
			vec4 vup = {0.0,1.0,0.0,0.0};
			lookAt(e,a,vup,model_view);
			angle+=0.001;
			if(z <= -0.5 ){
				stage=1;
				angle = M_PI;
			}
		}else{		
			GLfloat ex = 2.6*cos(angle) - 4.5;
			GLfloat ey = 3.4*sin(angle) + 4.0;
			vec4 e = {ex,ey,-0.5,0.0};
			GLfloat ax = 3.5*cos(angle)*cos(phi);
			GLfloat ay = 0.6 + 0.6*cos(angle)*sin(phi);
			GLfloat az = -4 - 3.5 * sin(angle);
			vec4 a = {ax,ay,az,0.0};
			vec4 vup = {0.0,1.0,0.0,0.0};
			lookAt(e,a,vup,model_view);
			angle+=0.005;
			phi+=0.005;
			if(angle > (3*M_PI)/2){//ex >= -4.52 || ey <= 0.75 ){
				move_down=0;
				vec4 e = {-4.5,0.6,-0.5,0.0};
				vec4 a = {-3.5,0.6,-0.5,0.0};
				vec4 vup = {0.0,1.0,0.0,0.0};
				lookAt(e,a,vup,model_view);	
				vec4 lrb = {-0.4,0.4,-0.4,0.0};
				vec4 tnf = {0.4,-0.5,-9,0.0};
				frustum(lrb,tnf,projection);
				vectorCopy(e,old_e);
				vectorCopy(a,old_a);
			}
		}
	} else if(forward){
		vec4 e_move, a_move;
		if(direction == 3){
			vec4 e_point = {old_e[0]+1,0.6,old_e[2],0.0};
			vec4 a_point = {old_a[0]+1,0.6,old_a[2],0.0};
			vectorSubtraction(e_point,old_e,e_move);
			vectorSubtraction(a_point,old_a,a_move);
		}else if(direction == 2){
			vec4 e_point = {old_e[0],0.6,old_e[2]-1,0.0};
			vec4 a_point = {old_a[0],0.6,old_a[2]-1,0.0};
			vectorSubtraction(e_point,old_e,e_move);
			vectorSubtraction(a_point,old_a,a_move);
		}else if(direction == 1){
			vec4 e_point = {old_e[0]-1,0.6,old_e[2],0.0};
			vec4 a_point = {old_a[0]-1,0.6,old_a[2],0.0};
			vectorSubtraction(e_point,old_e,e_move);
			vectorSubtraction(a_point,old_a,a_move);
		}else{
			vec4 e_point = {old_e[0],0.6,old_e[2]+1,0.0};
			vec4 a_point = {old_a[0],0.6,old_a[2]+1,0.0};
			vectorSubtraction(e_point,old_e,e_move);
			vectorSubtraction(a_point,old_a,a_move);
		}
		vectorScalarMultiplication(e_move,step,e_move);
		vectorScalarMultiplication(a_move,step,a_move);
		vectorAddition(old_e,e_move,e_move);
		vectorAddition(old_a,a_move,a_move);
		vec4 vup = {0,1,0,0};
		lookAt(e_move,a_move,vup,model_view);
		//printVector(a_move);
		step += 0.002;
		if(step > 1.0){
			vectorCopy(e_move, old_e);
			vectorCopy(a_move, old_a);
			forward = 0;
			step = 0.0;
		}
	} else if(right){
			GLfloat ax,az;
			if(direction == 2){
				ax = sin(angle) + old_e[0];
				az = -cos(angle) + old_e[2];
				new_direction = 3;
			}else if(direction == 1){
				ax = cos(angle) + old_e[0];
				az = -sin(angle) + old_e[2];
				new_direction = 2;
			}else if(direction == 3){
				ax = cos(angle) + old_e[0];
				az = sin(angle) + old_e[2];
				new_direction = 0;
			}else{
				ax = -sin(angle) + old_e[0];
				az = cos(angle) + old_e[2];
				new_direction = 1;
			}
			vec4 a = {ax,0.6,az,0.0};
			vec4 vup = {0.0,1.0,0.0,0.0};
			lookAt(old_e,a,vup,model_view);
			angle+=0.002;
			if(angle > M_PI/2){
				//vec4 e = {-4.5,0.6,-0.5,0.0};
				//vec4 a = {-3.5,0.6,-0.5,0.0};
				//vec4 vup = {0.0,1.0,0.0,0.0};
				//lookAt(e,a,vup,model_view);	
				right = 0;
				forward = 1;
				direction = new_direction;
				//vectorCopy(e,old_e);
				vectorCopy(a,old_a);
			}
	} else if(left){
			GLfloat ax,az;
			if(direction == 2){
				ax = -sin(angle) + old_e[0];
				az = cos(angle) + old_e[2];
				new_direction = 1;
			}else if(direction == 1){
				ax = cos(angle) + old_e[0];
				az = sin(angle) + old_e[2];
				new_direction = 0;
			}else if(direction == 3){
				ax = cos(angle) + old_e[0];
				az = -sin(angle) + old_e[2];
				new_direction = 2;
			}else{
				ax = sin(angle) + old_e[0];
				az = cos(angle) + old_e[2];
				new_direction = 3;
			}
			vec4 a = {ax,0.6,az,0.0};
			vec4 vup = {0.0,1.0,0.0,0.0};
			lookAt(old_e,a,vup,model_view);
			angle+=0.002;
			if(angle > M_PI/2){
				//vec4 e = {-4.5,0.6,-0.5,0.0};
				//vec4 a = {-3.5,0.6,-0.5,0.0};
				//vec4 vup = {0.0,1.0,0.0,0.0};
				//lookAt(e,a,vup,model_view);	
				left = 0;
				forward=1;
				direction = new_direction;
				//vectorCopy(e,old_e);
				vectorCopy(a,old_a);
			}
	} else{
		if(sol_count < 60){
			char c = solution[sol_count];
			angle = 0;
			if( c == '\0'){
				sol_count = 60;
				return;
			}else if(c == 'F'){
				if(last_f){
					last_f = 0;
					sol_count++;
					return;
				}
				forward = 1;
				last_f = 1;
			}else if(c == 'R'){
				right = 1;
				last_f =1;
			}else if(c == 'L'){
				left = 1;
				last_f=1;
			}
			sol_count++;
		}			
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Maze");
	glewInit();
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
