
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <random>
#include <time.h>
#include <stdlib.h> 
#include <string>
using namespace std;

SDL_Window* displayWindow;
/*
class GameApp
{
privite:
	bool done;
	float lastFrameTicks;
	float x_position_player_ship;
	float y_position_enemy_ship_line_1;
	float y_position_enemy_ship_line_2;
	float y_position_enemy_ship_line_3;
	float x_position_enemy_ship;
	int x_direction_enemy_ship;
	int block1;
	int block2;
	int block3;
	int enemy_fire_count_down;
	int score;
	time_t timeStart_enemy;
	time_t timeStart_player;
	vector<unsigned int> font;
	gameState scene;
	vector<SheetSprite> other_ship;
	vector<Bullet> bullets;
	vector<Enemy_ship> enemy_data;

public:

};*/
bool done = false;
float lastFrameTicks = 0.0f;
float x_position_player_ship = 0.0;
float y_position_enemy_ship_line_1 = 0.8;
float y_position_enemy_ship_line_2 = 0.6;
float y_position_enemy_ship_line_3 = 0.4;
float x_position_enemy_ship = 0.0;
int x_direction_enemy_ship = 1;
int block1 = 5;
int block2 = 5;
int block3 = 5;
int enemy_fire_count_down = 0.3;
int score = 0;
time_t timeStart_enemy = time(0);
time_t timeStart_player = time(0);
vector<unsigned int> font;
enum gameState{MENU,GAME,END};
gameState scene = MENU;


class SheetSprite {
public:
	SheetSprite() :
		textureID(0), u(0), v(v), width(0), height(0){}
	SheetSprite(unsigned int textureID, float u, float v, float width, float height) :
		textureID(textureID), u(u), v(v), width(width), height(height){}
	float scale;
	unsigned int textureID;
	float x;
	float y;
	float u;
	float v;
	float width;
	float height;

	void Draw(float x, float y, float rotation = 0.0, float scale = 1.0)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glTranslatef(x, y, 0.0);
		glRotatef(rotation, 0.0, 0.0, 1.0);
		GLfloat quad[] = { -width * scale, height * scale, -width * scale, -height * scale,
			width * scale, -height * scale, width * scale, height * scale };
		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);
		GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_QUADS, 0, 4);
		glDisable(GL_TEXTURE_2D);
	}
};

//vector<SheetSprite> enemy_ship; 
vector<SheetSprite> other_ship;

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(surface);
	return textureID;
}
void DrawText(GLuint fontTexture, string text, float size, float spacing, float r, float g, float b, float a, float x, float y) {
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float texture_size = 1.0 / 16.0f;
	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colorData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		colorData.insert(colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });
		vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) +
			(-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f
			* size });
		texCoordData.insert(texCoordData.end(), { texture_x, texture_y, texture_x, texture_y + texture_size, texture_x +
			texture_size, texture_y + texture_size, texture_x + texture_size, texture_y });
	}
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glColorPointer(4, GL_FLOAT, 0, colorData.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, text.size() * 4.0);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

class Bullet
{
public:
	float x;
	float y;
	float speed;
	int direction;
	Bullet() :x(0), y(0), speed(0), direction(0){}
	Bullet(float x, float y, float speed, int direction) :x(x), y(y), speed(speed), direction(direction){}
	void change_direction()
	{
		direction = (-direction);
	}
	void increment()
	{
		y = (y + speed*direction);
	}
	void draw()
	{
		other_ship[5].Draw(x, y, 0, 1);
	}

};
vector<Bullet> bullets;


class Enemy_ship
{
public:
	float x;
	float y;
	int index;
	int line;
	Enemy_ship() :x(0), y(0),index(0),line(0){}
	Enemy_ship(float x, float y, int index, int line) :x(x), y(y),index(index),line(line){}
	void update(float newx, float newy)
	{
		x = newx;
		y = newy;
	}
};
vector<Enemy_ship> enemy_data;

/*void draw_enemy_ship(SheetSprite enemy_ship, float x, float y)
{
	enemy_ship.Draw(x, y);
}*/

void player_fire()
{
	/*unsigned int testSheet = LoadTexture("sheet.png");
	SheetSprite bullet(testSheet, 856.0 / 1024.0, 421.0 / 1024.0, 9.0 / 1024.0, 54.0 / 1024.0);
	bullet.Draw(x_position_player_ship, -0.6, 0, 1);*/
	Bullet bullet(x_position_player_ship, -0.7, 0.00030, 1);
	bullets.push_back(bullet);
}

void enemy_fire()
{
	int index = rand() % enemy_data.size();
	while (enemy_data[index].line != 3)
		index = rand() % enemy_data.size();
	Bullet bullet(enemy_data[index].x, enemy_data[index].y-0.15, 0.00030, -1);
	bullets.push_back(bullet);
	
}

void collision_bullet_block()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if ((bullets[i].y) <= -0.4 && (bullets[i].y) >= -0.6)
		{
			if ((bullets[i].x) <= -0.4 && (bullets[i].x) >= -0.6)
			{
				if (block1 >= 1)
				{
					block1--;
					bullets.erase(bullets.begin() + i);
				}
			}
			else if ((bullets[i].x) <= 0.1 && (bullets[i].x) >= -0.1)
			{
				if (block2 >= 1)
				{
					block2--;
					bullets.erase(bullets.begin() + i);
				}
			}
			else if ((bullets[i].x) <= 0.6 && (bullets[i].x) >= 0.4)
			{
				if (block3 >= 1)
				{
					block3--;
					bullets.erase(bullets.begin() + i);
				}
			}
		}
	}
}

void collision_ship_block()
{
	for (int i = 0; i < enemy_data.size(); i++)
	{
		if ((enemy_data[i].y) <= -0.4 && (enemy_data[i].y) >= -0.6)
		{
			if ((enemy_data[i].x) <= -0.4 && (enemy_data[i].x) >= -0.6)
			{
				if (block1 >= 1)
				{
					enemy_data.erase(enemy_data.begin() + i);
					block1 = 0;
				}
			}
			else if ((enemy_data[i].x) <= 0.1 && (enemy_data[i].x) >= -0.1)
			{
				if (block2 >= 1)
				{
					enemy_data.erase(enemy_data.begin() + i);
					block2 = 0;
				}
			}
			else if ((enemy_data[i].x) <= 0.6 && (enemy_data[i].x) >= 0.4)
			{
				if (block3 >= 1)
				{
					enemy_data.erase(enemy_data.begin() + i);
					block3 = 0;
				}
			}
		}
	}
}

void collision_bullet_player()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if ((bullets[i].y) <= -0.7 && (bullets[i].y) >= -0.9)
		{
			if ((bullets[i].x <= (x_position_player_ship + 0.1)) && (bullets[i].x) >= (x_position_player_ship - 0.1))
			{
				scene = END;
				//bullets.erase(bullets.begin() + i);
				break;
			}
		}
	}
}

void collision_bullet_ship()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		for (int p = 0; p < enemy_data.size(); p++)
		{
			if (bullets[i].y <= enemy_data[p].y+0.07 && bullets[i].y >= enemy_data[p].y-0.07)
			{
				if ((bullets[i].x <= enemy_data[p].x + 0.07) && (bullets[i].x >= enemy_data[p].x - 0.07))
				{				
					enemy_data.erase(enemy_data.begin() + p);
					bullets.erase(bullets.begin() + i);
					score++;
					break;
				}
			}
		}
	}
}

void bullet_out_of_range()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i].y >= 1 || bullets[i].y <= -1)
		{
			bullets.erase(bullets.begin() + i);
		}
	}
}

void victory()
{
	if (enemy_data.size() == 0)
		scene = END;
}

void intialize()
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	SDL_Event event;
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glViewport(0, 0, 800, 600);
	unsigned int testSheet = LoadTexture("sheet.png");
	unsigned int font1 = LoadTexture("font1.png");
	SheetSprite player_ship(testSheet, 247.0 / 1024.0, 84.0 / 1024.0, 99.0 / 1024.0, 75.0 / 1024.0);
	SheetSprite block_1(testSheet, 224.0 / 1024.0, 748.0 / 1024.0, 101.0 / 1024.0, 84.0 / 1024.0);
	SheetSprite block_2(testSheet, 224.0 / 1024.0, 748.0 / 1024.0, 101.0 / 1024.0, 84.0 / 1024.0);
	SheetSprite block_3(testSheet, 224.0 / 1024.0, 748.0 / 1024.0, 101.0 / 1024.0, 84.0 / 1024.0);
	SheetSprite enemy_ship(testSheet, 423.0 / 1024.0, 728.0 / 1024.0, 93.0 / 1024.0, 84.0 / 1024.0);
	SheetSprite bullet(testSheet, 856.0 / 1024.0, 421.0 / 1024.0, 9.0 / 1024.0, 54.0 / 1024.0);
	other_ship.push_back(player_ship);
	other_ship.push_back(block_1);
	other_ship.push_back(block_2);
	other_ship.push_back(block_3);
	other_ship.push_back(enemy_ship);
	other_ship.push_back(bullet);
	font.push_back(font1);
	for (int i = 0; i < 7; i++)
	{
		enemy_data.push_back(Enemy_ship(float(float(i)*0.2 - 0.7), y_position_enemy_ship_line_1,i,1));
	}
	for (int i = 0; i < 7; i++)
	{
		enemy_data.push_back(Enemy_ship(float(float(i)*0.2 - 0.7), y_position_enemy_ship_line_2,i,2));
	}
	for (int i = 0; i < 7; i++)
	{
		enemy_data.push_back(Enemy_ship(float(float(i)*0.2 - 0.7), y_position_enemy_ship_line_3,i,3));
	}
}

void render(SDL_Event& event)
{
	while (SDL_PollEvent(&event)) 
	{
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
		{
			done = true;
		}
	}
	if (scene == MENU)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		DrawText(font[0], "SPACE INVADER", 0.15, 0.0, 1.0, 1.0, 1.0, 1.0, -0.9, 0.3);
		DrawText(font[0], "PRESS SPACE", 0.1, 0.0, 1.0, 1.0, 1.0, 1.0, -0.5, -0.3);
		DrawText(font[0], "TO START", 0.1, 0.0, 1.0, 1.0, 1.0, 1.0, -0.35, -0.5);
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_V)
			{
				scene = GAME;
			}
		}
	}
	if (scene == GAME)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)//P1 controls
			{
				if (x_position_player_ship <= 0.92)//prevent the paddle go out of the screen
				{
					other_ship[0].Draw(x_position_player_ship += 0.0005, -0.8, 0, 1);
				}
				else
				{
					other_ship[0].Draw(x_position_player_ship, -0.8, 0, 1);
				}
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
			{
				if (x_position_player_ship >= -0.92)//prevent the paddle go out of the screen
				{
					other_ship[0].Draw(x_position_player_ship -= 0.0005, -0.8, 0, 1);
				}
				else
				{
					other_ship[0].Draw(x_position_player_ship, -0.8, 0, 1);
				}
			}
			/*if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
			{
			other_ship[0].Draw(x_position_player_ship, -0.8, 0, 1);
			player_fire();
			}*/
		}
		else
		{
			other_ship[0].Draw(x_position_player_ship, -0.8, 0, 1);
		}
		glClear(GL_COLOR_BUFFER_BIT);
		DrawText(font[0], "SCORE", 0.1, 0.00005, 1.0, 1.0, 1.0, 1.0, -0.7, 0.9);
		DrawText(font[0], to_string(score), 0.1, 0.00005, 1.0, 1.0, 1.0, 1.0, -0.1, 0.9);
		other_ship[0].Draw(x_position_player_ship, -0.8, 0, 1);
		if (block1 >= 1)
			other_ship[1].Draw(-0.5, -0.5, 0, 1.0);
		if (block2 >= 1)
			other_ship[2].Draw(0.0, -0.5, 0, 1.0);
		if (block3 >= 1)
			other_ship[3].Draw(0.5, -0.5, 0, 1.0);
		for (int i = 0; i < enemy_data.size(); i++)
		{
			other_ship[4].Draw(enemy_data[i].x, enemy_data[i].y, 0, 0.8);
		}
		for (int i = 0; i < bullets.size(); i++)
		{
			bullets[i].increment();
			bullets[i].draw();
		}
	}
	if (scene == END)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		DrawText(font[0], "THANKS", 0.1, 0.00005, 1.0, 1.0, 1.0, 1.0, -0.3, 0.3);
		DrawText(font[0], "FOR", 0.1, 0.00005, 1.0, 1.0, 1.0, 1.0, -0.15, 0.1);
		DrawText(font[0], "PLAY", 0.1, 0.00005, 1.0, 1.0, 1.0, 1.0, -0.2, -0.1);
		DrawText(font[0], "PRESS SPACE TO START AGAIN", 0.05, 0, 1.0, 1.0, 1.0, 1.0, -0.6, -0.3);
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_V)
			{
				scene = GAME;
			}
		}
	}
	SDL_GL_SwapWindow(displayWindow);
}

void update()
{
	if (scene == GAME)
	{
		time_t time_passed_enemy = time(0);
		if ((time_passed_enemy - timeStart_enemy) >= 1.5)
		{
			enemy_fire();
			timeStart_enemy = time(0);
		}
		time_t time_passed_player = time(0);
		if ((time_passed_player - timeStart_player) >= 1.0)
		{
			player_fire();
			timeStart_player = time(0);
		}
		victory();
		collision_bullet_block();
		collision_ship_block();
		collision_bullet_player();
		collision_bullet_ship();
		bullet_out_of_range();
		y_position_enemy_ship_line_1 -= 0.000005;
		y_position_enemy_ship_line_2 -= 0.000005;
		y_position_enemy_ship_line_3 -= 0.000005;
		if (x_position_enemy_ship >= 0.4)
		{
			x_direction_enemy_ship = -1;
		}
		else if (x_position_enemy_ship <= -0.2)
		{
			x_direction_enemy_ship = 1;
		}
		x_position_enemy_ship += x_direction_enemy_ship*0.00005;
		for (int i = 0; i < enemy_data.size(); i++)
		{
			if (enemy_data[i].line == 3)
				enemy_data[i].update(float(enemy_data[i].index*0.2 - 0.7) + x_position_enemy_ship, y_position_enemy_ship_line_3);
			if (enemy_data[i].line == 2)
				enemy_data[i].update(float(enemy_data[i].index*0.2 - 0.7) + x_position_enemy_ship, y_position_enemy_ship_line_2);
			if (enemy_data[i].line == 1)
				enemy_data[i].update(float(enemy_data[i].index*0.2 - 0.7) + x_position_enemy_ship, y_position_enemy_ship_line_1);
		}
	}
}

int main(int argc, char *argv[])
{	
	intialize();
	SDL_Event event;
	while (!done)
	{
		render(event);
		update();
	}
	SDL_Quit();
	return 0;
}