/*
This is the C++ project of CS3113 Assignment #2
Space Invader
Profesoor Ivan Safrin
Author Beihong Chen


*The files in folder named backups are not neccessary to use for the program.*

Victory condition :
Destory all enemy ships.

Controll :
		 1. RIGHT for moving right
		 2. LEFT for moving left
		 3. UP on main menu to enter the game
		 4. DOWN on end menu to restart the game
		 5. Both enemy ships and player ship can fire steady.No need to press any buttons.
		 6. Approximately, 1.5 second gap between player fire and 1.0 second between enemy fire.

		 Defeated condition :
1. Hit by an enemy ship.
2. Hit by a bullet fired by an enemy ship.
3. One of the enemy ship goes below the screen, y <= -1.0

Other notice :
1.All the rocks / blocks are 5 lifes which are able to absorb 5 bullets fired both by enemies and player.
2.If any enemy hits blocks, the ship will destory and the blocks will do so.
3.I made bullet bullet collision dectection but seems there is some memory issues there, so I did not use it, however, according to my design, its super difficult to make a such collsion.
*/



#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <random>
#include <time.h>
#include <stdlib.h> 
#include <string>
using namespace std;


enum gameState{ MENU, GAME, END };


class SheetSprite 
{
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

void DrawSprite(GLint texture, float x, float y, float rotation = 9.0) 
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -0.15, 0.15f, -0.15f, -0.15f, 0.15f, -0.15f, 0.15f, 0.15f };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
}

GLuint LoadTexture(const char *image_path) 
{
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

void DrawText(GLuint fontTexture, string text, float size, float spacing, float r, float g, float b, float a, float x, float y) 
{
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float texture_size = 1.0 / 16.0f;
	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colorData;
	for (int i = 0; i < text.size(); i++) 
	{
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

class Explosion //this class makes entities of explosions while bullets hit ships/player/rocks
{
public:
	float x;
	float y;
	float time;
	int type;
	Explosion() :x(0), y(0), time(0),type(0){}
	Explosion(float x, float y, float time,int type) :x(x), y(y), time(time),type(type){}
};

class Bullet //this class makes entities of bullets fired by enemy/player
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
};

class Enemy_ship //this class makes entities of enemy ship
{
public:
	float x;
	float y;
	int index;
	int line;
	Enemy_ship() :x(0), y(0), index(0), line(0){}
	Enemy_ship(float x, float y, int index, int line) :x(x), y(y), index(index), line(line){}
	void update(float newx, float newy)
	{
		x = newx;
		y = newy;
	}
};


class Game //The game class
{
private:
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
	int survive;
	float timeStart_enemy;
	float timeStart_player;
	float elapsed;
	vector<unsigned int> font;
	vector<Enemy_ship*> enemy_data;
	vector<Bullet*> bullets;
	vector<Explosion*> explodes;
	vector<SheetSprite> other_ship;
	gameState scene;
	SDL_Event event;
	SDL_Window* displayWindow;

public:
	void collision_bullet_block(); //detect collsion when a bullet hits a rock, explosion happens and remove the bullet and reduce the health of rock by 1
	void collision_bullet_ship(); //detect collsions when a bullet(by player) hits an enemy ship, removes both bullet and ship
	void collision_ship_block(); //detect collsions when an enemy ship hits the rock, remove both enemy ship and rock
	void collision_bullet_player(); //detect collsions when a bullet hits player, set survive to -1
	void collision_ship_player(); //detect collsions when an enemy ship hits player, set survive to -1
	void collision_bullet_bullet(); //detect collsions when a bullet hits another bullet, remove both bullets
	void explosion(float x, float y, float t, int type); //detect every time a collsion happens and make Explosion entities
	void ship_out_of_range(); //detect if any ship goes out of the screen, set survive to -1
	void bullet_out_of_range(); //detect if any bullet goes out of the screen, remove bullet
	void explosion_out_of_time(); //detect if any explosion is out of time, for ship explosion, it consists for 1.5 second and for rock explosion it consists for 1.0s
	void enemy_fire(); //let a random enemy ship fire a bullet at the current position
	void player_fire(); //let the player fire a bullet at the current position
	void victory(); //detect whether there is no more enemy ships or the survive is -1, if so ,set scene to END
	void increment(); //the main increment function, add all the values correspondingly by elasped and ticks
	void intialize(); //intialize all the variable and set all the necessary containers in the beginning of the game
	void reset(); //reset all the variables and empty all the containers when a new round is started
	void render(); //draw sprites and detect all input actions
	void update(); //update all the necessary variables
	void MENU_render(); //render function when the scene is Menu
	void GAME_render(); //render function when the scene is at Game
	void END_render(); //render function when the scene is End
	bool is_done() //detect whether done is true, if true, end the program main while loop
	{
		return done;
	}
};

void Game::explosion_out_of_time()
{
	for (int i = 0; i < explodes.size(); i++)
	{
		if (explodes[i]->time <= 2)
		{
			Explosion *tmp = explodes[i];
			explodes.erase(explodes.begin() + i);
			delete tmp;
		}
	}
}

void Game::explosion(float x, float y, float t, int type)
{
	Explosion *explosion = new Explosion(x, y, t, type);
	explodes.push_back(explosion);
}

void Game::player_fire()
{
	Bullet *bullet = new Bullet(x_position_player_ship, -0.7, 1.5, 1);
	bullets.push_back(bullet);
}

void Game::enemy_fire()
{
	int index = rand() % enemy_data.size();
	Bullet* bullet = new Bullet(enemy_data[index]->x, enemy_data[index]->y-0.15, 1.5, -1);
	bullets.push_back(bullet);
	
}

void Game::collision_ship_player()
{
	for (int i = 0; i < enemy_data.size(); i++)
	{
		if (enemy_data[i]->y <= -0.7 && enemy_data[i]->y >= -0.9)
		{
			if (enemy_data[i]->x <= (x_position_player_ship + 0.1) && enemy_data[i]->x >= (x_position_player_ship - 0.1))
			{
				explosion(enemy_data[i]->x, enemy_data[i]->y, 3.0, 1);
				survive = -1;
			}
		}
	}
}

void Game::ship_out_of_range()
{
	for (int i = 0; i < enemy_data.size(); i++)
	{
		if (enemy_data[i]->y <= -1.0)
		{
			survive = -1;
			explosion(x_position_player_ship, -0.8, 3.0, 1);
			break;
		}
	}
}

void Game::collision_bullet_bullet()
{
	int done = 0;
	for (int i = 0; i < bullets.size(); i++)
	{
		for (int p = 0; p < bullets.size(); p++)
		{
			if (bullets[i]->x <= bullets[p]->x + 0.05 && bullets[i]->x >= bullets[p]->x - 0.05)
			{
				if (bullets[i]->y <= bullets[p]->y + 0.05 && bullets[i]->y >= bullets[p]->y - 0.05)
				{
					Bullet* tmp_1 = bullets[i];
					Bullet* tmp_2 = bullets[p];
					bullets.erase(bullets.begin() + i);
					if (i < p)
						bullets.erase(bullets.begin() + p - 1);
					else
						bullets.erase(bullets.begin() + p);
					delete tmp_1;
					delete tmp_2;
					done = 1;
					break;
				}
			}
		}
		if (done == 1)
			break;
	}
}

void Game::collision_bullet_block()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if ((bullets[i]->y) <= -0.4 && (bullets[i]->y) >= -0.6)
		{
			if ((bullets[i]->x) <= -0.4 && (bullets[i]->x) >= -0.6)
			{
				if (block1 >= 1)
				{
					block1--;
					explosion(bullets[i]->x,bullets[i]->y, 2.5, 2);
					Bullet* tmp = *(bullets.begin() + i);
					bullets.erase(bullets.begin() + i);
					delete tmp;
				}
			}
			else if ((bullets[i]->x) <= 0.1 && (bullets[i]->x) >= -0.1)
			{
				if (block2 >= 1)
				{
					block2--;
					explosion(bullets[i]->x, bullets[i]->y, 2.5, 2);
					Bullet* tmp = *(bullets.begin() + i);
					bullets.erase(bullets.begin() + i);
					delete tmp;
				}
			}
			else if ((bullets[i]->x) <= 0.6 && (bullets[i]->x) >= 0.4)
			{
				if (block3 >= 1)
				{
					block3--;
					explosion(bullets[i]->x, bullets[i]->y, 2.5, 2);
					Bullet* tmp = *(bullets.begin() + i);
					bullets.erase(bullets.begin() + i);
					delete tmp;
				}
			}
		}
	}
}

void Game::collision_ship_block()
{
	for (int i = 0; i < enemy_data.size(); i++)
	{
		if ((enemy_data[i]->y) <= -0.4 && (enemy_data[i]->y) >= -0.6)
		{
			if ((enemy_data[i]->x) <= -0.4 && (enemy_data[i]->x) >= -0.6)
			{
				if (block1 >= 1)
				{
					explosion(enemy_data[i]->x, enemy_data[i]->y, 3.0, 1);
					Enemy_ship* tmp = *(enemy_data.begin() + i);
					enemy_data.erase(enemy_data.begin() + i);
					block1 = 0;
					delete tmp;
				}
			}
			else if ((enemy_data[i]->x) <= 0.1 && (enemy_data[i]->x) >= -0.1)
			{
				if (block2 >= 1)
				{
					explosion(enemy_data[i]->x, enemy_data[i]->y, 3.0, 1);
					Enemy_ship* tmp = *(enemy_data.begin() + i);
					enemy_data.erase(enemy_data.begin() + i);
					block2 = 0;
					delete tmp;
				}
			}
			else if ((enemy_data[i]->x) <= 0.6 && (enemy_data[i]->x) >= 0.4)
			{
				if (block3 >= 1)
				{
					explosion(enemy_data[i]->x, enemy_data[i]->y, 3.0, 1);
					Enemy_ship* tmp = *(enemy_data.begin() + i);
					enemy_data.erase(enemy_data.begin() + i);
					block3 = 0;
					delete tmp;
				}
			}
		}
	}
}

void Game::collision_bullet_player()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if ((bullets[i]->y) <= -0.7 && (bullets[i]->y) >= -0.9)
		{
			if ((bullets[i]->x <= (x_position_player_ship + 0.1)) && (bullets[i]->x) >= (x_position_player_ship - 0.1))
			{
				explosion(x_position_player_ship,-0.8, 3.0,1);
				survive = -1;
				break;
			}
		}
	}
}

void Game::collision_bullet_ship()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i]->direction == 1)
		{
			for (int p = 0; p < enemy_data.size(); p++)
			{
				if (bullets[i]->y <= enemy_data[p]->y + 0.07 && bullets[i]->y >= enemy_data[p]->y - 0.07)
				{
					if ((bullets[i]->x <= enemy_data[p]->x + 0.07) && (bullets[i]->x >= enemy_data[p]->x - 0.07))
					{
						Bullet* bullet_tmp = *(bullets.begin() + i);
						Enemy_ship* ship_tmp = *(enemy_data.begin() + p);
						explosion(enemy_data[p]->x, enemy_data[p]->y,3.0,1);
						enemy_data.erase(enemy_data.begin() + p);
						bullets.erase(bullets.begin() + i);
						score++;
						delete bullet_tmp;
						delete ship_tmp;
						break;
					}
				}
			}
		}
	}
}

void Game::bullet_out_of_range()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i]->y >= 1 || bullets[i]->y <= -1)
		{
			Bullet* bullet_tmp = *(bullets.begin() + i);
			bullets.erase(bullets.begin() + i);
			delete bullet_tmp;
		}
	}
}

void Game::victory()
{
	if (enemy_data.size() == 0 || survive == -1)
	{
		scene = END;
	}
}

void Game::increment()
{
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	y_position_enemy_ship_line_1 -= elapsed*0.015;
	y_position_enemy_ship_line_2 -= elapsed*0.015;
	y_position_enemy_ship_line_3 -= elapsed*0.015;
	if (x_position_enemy_ship >= 0.4)
	{
		x_direction_enemy_ship = -1;
	}
	else if (x_position_enemy_ship <= -0.2)
	{
		x_direction_enemy_ship = 1;
	}
	x_position_enemy_ship += x_direction_enemy_ship*elapsed*0.05;
	for (int i = 0; i < enemy_data.size(); i++)
	{
		if (enemy_data[i]->line == 3)
			enemy_data[i]->update(float(enemy_data[i]->index*0.2 - 0.7) + x_position_enemy_ship, y_position_enemy_ship_line_3);
		if (enemy_data[i]->line == 2)
			enemy_data[i]->update(float(enemy_data[i]->index*0.2 - 0.7) + x_position_enemy_ship, y_position_enemy_ship_line_2);
		if (enemy_data[i]->line == 1)
			enemy_data[i]->update(float(enemy_data[i]->index*0.2 - 0.7) + x_position_enemy_ship, y_position_enemy_ship_line_1);
	}
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->y += bullets[i]->direction*elapsed*bullets[i]->speed;
	}
	for (int i = 0; i < explodes.size(); i++)
	{
		explodes[i]->time -= elapsed;
	}
}

void Game::reset()
{
	for (int i = 0; i < enemy_data.size(); i++)
	{
		Enemy_ship* tmp = enemy_data[i];
		enemy_data.erase(enemy_data.begin() + i);
		delete tmp;
	}
	for (int i = 0; i < bullets.size(); i++)
	{
		Bullet* tmp = bullets[i];
		bullets.erase(bullets.begin() + i);
		delete tmp;
	}
	for (int i = 0; i < explodes.size(); i++)
	{
		Explosion* tmp = explodes[i];
		explodes.erase(explodes.begin() + i);
		delete tmp;
	}
	enemy_data.clear();
	bullets.clear();
	explodes.clear();
	x_position_player_ship = 0.0;
	y_position_enemy_ship_line_1 = 0.8;
	y_position_enemy_ship_line_2 = 0.6;
	y_position_enemy_ship_line_3 = 0.4;
	x_position_enemy_ship = 0.0;
	x_direction_enemy_ship = 1;
	block1 = 5;
	block2 = 5;
	block3 = 5;
	survive = 1;
	enemy_fire_count_down = 0.3;
	score = 0;
	timeStart_enemy = 1.0;
	timeStart_player = 1.5;
	scene = MENU;
	for (int i = 0; i < 7; i++)
	{
		Enemy_ship *tmp = new Enemy_ship(float(float(i)*0.2 - 0.7), y_position_enemy_ship_line_1, i, 1);
		enemy_data.push_back(tmp);
	}
	for (int i = 0; i < 7; i++)
	{
		Enemy_ship *tmp = new Enemy_ship(float(float(i)*0.2 - 0.7), y_position_enemy_ship_line_2, i, 2);
		enemy_data.push_back(tmp);
	}
	for (int i = 0; i < 7; i++)
	{
		Enemy_ship *tmp = new Enemy_ship(float(float(i)*0.2 - 0.7), y_position_enemy_ship_line_3, i, 3);
		enemy_data.push_back(tmp);
	}
}

void Game::intialize()
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glViewport(0, 0, 800, 600);
	done = false;
	//lastFrameTicks = 0.0f;
	//elapsed = 0.0f;
	x_position_player_ship = 0.0;
	y_position_enemy_ship_line_1 = 0.8;
	y_position_enemy_ship_line_2 = 0.6;
	y_position_enemy_ship_line_3 = 0.4;
	x_position_enemy_ship = 0.0;
	x_direction_enemy_ship = 1;
	block1 = 5;
	block2 = 5;
	block3 = 5;
	survive = 1;
	enemy_fire_count_down = 0.3;
	score = 0;
	timeStart_enemy = 1.5;
	timeStart_player = 1.0;
	scene = MENU;
	unsigned int testSheet = LoadTexture("sheet.png");
	unsigned int font1 = LoadTexture("font1.png");
	unsigned int explosion = LoadTexture("Explosion.png");
	SheetSprite player_ship(testSheet, 247.0 / 1024.0, 84.0 / 1024.0, 99.0 / 1024.0, 75.0 / 1024.0);
	SheetSprite block_1(testSheet, 224.0 / 1024.0, 748.0 / 1024.0, 101.0 / 1024.0, 84.0 / 1024.0);
	SheetSprite block_2(testSheet, 224.0 / 1024.0, 748.0 / 1024.0, 101.0 / 1024.0, 84.0 / 1024.0);
	SheetSprite block_3(testSheet, 224.0 / 1024.0, 748.0 / 1024.0, 101.0 / 1024.0, 84.0 / 1024.0);
	SheetSprite enemy_ship(testSheet, 423.0 / 1024.0, 728.0 / 1024.0, 93.0 / 1024.0, 84.0 / 1024.0);
	SheetSprite bullet(testSheet, 856.0 / 1024.0, 421.0 / 1024.0, 9.0 / 1024.0, 54.0 / 1024.0);
	SheetSprite explosion_rock(testSheet, 576.0/1024.0, 300.0/1024.0, 24.0/1024.0, 24.0/1024.0); 
	other_ship.push_back(player_ship);
	other_ship.push_back(block_1);
	other_ship.push_back(block_2);
	other_ship.push_back(block_3);
	other_ship.push_back(enemy_ship);
	other_ship.push_back(bullet);
	other_ship.push_back(explosion_rock);
	font.push_back(font1);
	font.push_back(explosion);
	for (int i = 0; i < 7; i++)
	{
		Enemy_ship *tmp = new Enemy_ship(float(float(i)*0.2 - 0.7), y_position_enemy_ship_line_1, i, 1);
		enemy_data.push_back(tmp);
	}
	for (int i = 0; i < 7; i++)
	{
		Enemy_ship *tmp = new Enemy_ship(float(float(i)*0.2 - 0.7), y_position_enemy_ship_line_2, i, 2);
		enemy_data.push_back(tmp);
	}
	for (int i = 0; i < 7; i++)
	{
		Enemy_ship *tmp = new Enemy_ship(float(float(i)*0.2 - 0.7), y_position_enemy_ship_line_3, i, 3);
		enemy_data.push_back(tmp);
	}
}

void Game::MENU_render()
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.scancode == SDL_SCANCODE_UP)
		{
			scene = GAME;
		}
	}	
	glClear(GL_COLOR_BUFFER_BIT);
	DrawText(font[0], "SPACE INVADER", 0.15, 0.0, 1.0, 1.0, 1.0, 1.0, -0.9, 0.3);
	DrawText(font[0], "PRESS UP", 0.1, 0.0, 1.0, 1.0, 1.0, 1.0, -0.35, -0.3);
	DrawText(font[0], "TO START", 0.1, 0.0, 1.0, 1.0, 1.0, 1.0, -0.35, -0.5);
	SDL_GL_SwapWindow(displayWindow);
}

void Game::GAME_render()
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
		{
			if (x_position_player_ship <= 0.92)
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
			if (x_position_player_ship >= -0.92)
			{
				other_ship[0].Draw(x_position_player_ship -= 0.0005, -0.8, 0, 1);
			}
			else
			{
				other_ship[0].Draw(x_position_player_ship, -0.8, 0, 1);
			}
		}
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
		other_ship[4].Draw(enemy_data[i]->x, enemy_data[i]->y, 0, 0.8);
	}
	for (int i = 0; i < bullets.size(); i++)
	{
		other_ship[5].Draw(bullets[i]->x, bullets[i]->y, 0, 1.0);
	}
	for (int i = 0; i < explodes.size(); i++)
	{
		if (explodes[i]->type == 1)
			DrawSprite(font[1], explodes[i]->x, explodes[i]->y);
		else if (explodes[i]->type == 2)
			other_ship[6].Draw(explodes[i]->x, explodes[i]->y, 0, 1.0);
	}
	SDL_GL_SwapWindow(displayWindow);
}

void Game::END_render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	DrawText(font[0], "THANKS", 0.1, 0.00005, 1.0, 1.0, 1.0, 1.0, -0.3, 0.3);
	DrawText(font[0], "FOR", 0.1, 0.00005, 1.0, 1.0, 1.0, 1.0, -0.15, 0.1);
	DrawText(font[0], "PLAY", 0.1, 0.00005, 1.0, 1.0, 1.0, 1.0, -0.2, -0.1);
	DrawText(font[0], "PRESS DOWN TO START AGAIN", 0.05, 0, 1.0, 1.0, 1.0, 1.0, -0.6, -0.3);
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
		{
			reset();
		}
	}
	SDL_GL_SwapWindow(displayWindow);
}

void Game::render()
{
	while (SDL_PollEvent(&event)) 
	{
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
		{
			done = true;
		}
	}
	switch (scene)
	{
		case (MENU) :
		{
			MENU_render();
			break;
		}
		case (GAME) :
		{
			GAME_render();
			break;
		}
		case (END):
		{
			END_render();
			break;
		}
	}
}

void Game::update()
{
	if (scene == MENU)
	{
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
	}
	if (scene == GAME)
	{
		timeStart_enemy -= elapsed;
		if ((timeStart_enemy) <= 0.0)
		{
			enemy_fire();
			timeStart_enemy = 1.0;
		}
		timeStart_player -= elapsed;
		if ((timeStart_player) <= 0.0)
		{
			player_fire();
			timeStart_player = 1.5;
		}
		victory();
		explosion_out_of_time();
		collision_bullet_block();
		collision_ship_block();
		collision_bullet_player();
		collision_bullet_ship();
		collision_ship_player();
		//collision_bullet_bullet();
		ship_out_of_range();
		bullet_out_of_range();
		increment();
	}
}

int main(int argc, char *argv[])
{	
	Game Space_Invader;
	Space_Invader.intialize();
	while (!Space_Invader.is_done())
	{
		Space_Invader.render();
		Space_Invader.update();
	}
	SDL_Quit();
	return 0;
}