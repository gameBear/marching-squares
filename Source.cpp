#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
static int screenW = 1000, screenH = 1000;
const int mapSize = 100;
int tileSize = 10;

int wrap(int i) {
	while (i >= mapSize) i -= mapSize;
	while (i < 0) i += mapSize;
	return i;
}
/////////////////////////////////// THIS IS FOR ALL THE STUFF YOU NORMALY DO IN THE PGE.... /////////////////////////////////
class MainPGE : public olc::PixelGameEngine
{
public:
	bool bMap[mapSize][mapSize];
	bool xMap[mapSize][mapSize];
	int iMap[mapSize][mapSize];
	bool grav = false;
	int tileTypes[3] = { 4,8,10 };
	int mapSelect = 0;
	olc::Sprite *tiles[3];
	MainPGE()
	{
		sAppName = "This is the main window";
	}

	/////////////////////////////////////// COUNTS WHAT TILE SHOULD BE DRAWN //////////////////////

	int count(int x, int y) {
		int i = 0;
		if (bMap[wrap(x)][wrap(y)]) i += 1;
		if (bMap[wrap(x+1)][wrap(y)]) i += 2;
		if (y < mapSize - 1) {
			if (bMap[wrap(x)][wrap(y + 1)]) i += 4;
			if (bMap[wrap(x + 1)][wrap(y + 1)]) i += 8;
		}
		else {
			if(bMap[wrap(x)][wrap(y)]) i += 4;
			if (bMap[wrap(x + 1)][wrap(y)]) i += 8;
		}
		return i;
	}

	///////////////////////////////// UPDATE THE TILEMAP INDEXES ////////////////////////////////
	void setIntMap() {
		for (int i = 0; i < mapSize; i++) {
			for (int j = 0; j < mapSize; j++) {
				iMap[i][j] = count(i, j);
				xMap[i][j] = false;
			}
		}
	}

	//////////////////////////////// MAKE RANDOM MAP AND TURN OFF ANY CHANGES ///////////////////////////////////
	void randMap() {
		int val = rand() % 9+ 2;
		for (int i = 0; i < mapSize; i++) {
			for (int j = 0; j < mapSize; j++) {
				if(rand() % val == 0) bMap[i][j] = true;
				else bMap[i][j] = false;
				xMap[i][j] = false;
			}
		}
		grav = false;
	}


	////////////////////////////////////// FALKE GRAVITY PHYSICS ///////////////////////////////////////////
	void fallAlgorithm() {
		for (int j = 0; j < mapSize - 1; j++) {
			for (int i = 0; i < mapSize; i++) {
				if (bMap[i][j] && !xMap[i][j]) {
					int r = rand() % 3 - 1;
					if (!bMap[wrap(i)][j + 1]) {
						bMap[i][j] = false;
						bMap[wrap(i)][j + 1] = true;
						xMap[wrap(i)][j + 1] = true;
					}
					else if (!bMap[wrap(i + r)][j + 1] && rand() % 7 == 0) {
						bMap[i][j] = false;
						bMap[wrap(i + r)][j + 1] = true;
						xMap[wrap(i + r)][j + 1] = true;
					}
					else if (!bMap[wrap(i + r * 2)][j + 1] && !bMap[wrap(i + r)][j] && rand() % 20 == 0) {
						bMap[i][j] = false;
						bMap[wrap(i + r)][j] = true;
						xMap[wrap(i + r)][j] = true;
					}
					else if (rand() % 10000000 < j / 20) {
						bool done = false;
						int count = 0;
						while (!done && count < mapSize) {
							int x = rand() % mapSize;
							count++;
							if (!bMap[x][0]) {
								bMap[i][j] = false;
								bMap[x][0] = true;
								done = true;
								xMap[x][0] = true;
							}
						}
					}
				}
			}

		}
	}

	/////////////////////////////////////////////////////// JUST SEMI RANDOM GROWTH //////////////////////////////////////////
	void growthAlgorithm() {
		{
			for (int k = 0; k < mapSize; k++) {
				int i = rand() % mapSize;
				int j = rand() % mapSize;
				if (bMap[i][j]) {
					int r1 = rand() % 3 - 1;
					int r2 = rand() % 3 - 1;
					if (r1 != 0 || r2 != 0 || rand() % 5 == 0) bMap[wrap(i + r1)][wrap(j + r2)] = !bMap[wrap(i + r1)][wrap(j + r2)];
				}
			}
		}
	}

	//////////////////////////////////////////////// GAME OF LIFE /////////////////////////////////////////////////////////////////
	bool golCount(int x, int y) {
		int r = 0;
		for (int i = -1; i < 2; i++) for (int j = -1; j < 2; j++) if (bMap[wrap(x + i)][wrap(y + j)] && (i != 0 || j != 0)) r++;
		if (bMap[x][y]) {
			if (r == 2 || r == 3) return true;
			return false;
		}
		else if (r == 3) return true;
		return false;
	}

	void gameOfLife() {
		{
			bool tMap[mapSize][mapSize];
			for (int j = 0; j < mapSize; j++)
				for (int i = 0; i < mapSize; i++)
					tMap[i][j] = golCount(i, j);

			for (int j = 0; j < mapSize; j++)
				for (int i = 0; i < mapSize; i++)
					bMap[i][j] = tMap[i][j];
		}
	}

	///////////////////////////////////////////////// UPDATE MAP /////////////////////////////////////////////////////////////////////
	void updateMap(float fElapsedTime) {
		static float timer = 0;
		timer += fElapsedTime;
		if (timer > 1.0f / 50 || !grav) {
			timer = 0;
			if (grav && mapSelect == 2) fallAlgorithm();
			else if (grav && mapSelect == 1) growthAlgorithm();

			else if (grav && mapSelect == 0) gameOfLife();
			setIntMap();
		}
	}

	void DrawMap() {
		int offSet = (1000 - mapSize * tileSize) * 0.5;
		Clear(olc::VERY_DARK_BLUE);
		for (int i = 0; i < mapSize; i++) {
			for (int j = 0; j < mapSize; j++) {
				DrawPartialSprite(i * tileSize + offSet, j * tileSize + offSet, tiles[mapSelect], (iMap[i][j] % 4) * tileSize, (int)(iMap[i][j] / 4) *tileSize, tileSize, tileSize);
			}
		}
	}
	bool OnUserCreate() override
	{
		tiles[0] = new olc::Sprite("tiles.png");
		tiles[1] = new olc::Sprite("tiles8.png");
		tiles[2] = new olc::Sprite("tiles10.png");
		mapSelect = rand() % 3;
		tileSize = tileTypes[mapSelect];
		randMap();
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{

		if (GetKey(olc::Key::ENTER).bPressed)randMap();
		if (GetKey(olc::Key::SPACE).bPressed) {
			mapSelect++;
			if (mapSelect >= 3) mapSelect = 0;
			tileSize = tileTypes[mapSelect];
		}
		if (GetKey(olc::Key::G).bPressed)grav = !grav;
		if (GetMouse(0).bHeld) {
			int offSet = (1000 - mapSize * tileSize) * 0.5;
			bMap[(GetMouseX() - offSet) / tileSize][(GetMouseY() - offSet) / tileSize] = true;
		}
		if (GetMouse(1).bHeld) {
			int offSet = (1000 - mapSize * tileSize) * 0.5;
			bMap[(GetMouseX()- offSet) / tileSize][(GetMouseY() - offSet) / tileSize] = false;
		}
		updateMap(fElapsedTime);


		DrawMap();
		return true;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{

	MainPGE MainSystem;
	if (MainSystem.Construct(screenW, screenH, 1, 1, false))
		MainSystem.Start();
	return 0;
}
