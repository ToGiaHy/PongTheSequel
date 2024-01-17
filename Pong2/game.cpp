#include <string>
#include<cstdlib>
#include <map>
#include <vector>
#include <windows.h>
#include <thread>
#include "resource.cpp"
#include <stdio.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <ctime>
#include <algorithm> // Include for sorting
#include <string>
//music switches
std::atomic<bool> loadingFinished(false);
static bool playMusic = false;
static bool playMusic1 = false;
static bool playMusic2 = false;
static bool playMusic3 = false;
static bool playMusic4 = false;
static bool playMusic5 = false;
static bool playMusic6 = false;

//button input states
static bool quit = false;
#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

//game object properties
float player_1_p, player_1_dp, player_2_p, player_2_dp, player_3_p = 0, player_3_dp, player_4_p = 0, player_4_dp;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;
float ball_p_x = 0, ball_p_y = 0, ball_dp_x = 130, ball_dp_y, ball_half_size = 2;
float ball_p1_x = 0, ball_p1_y = 0, ball_dp1_x = 130, ball_dp1_y, ball_half1_size = 2;
float timer;
int player_1_score, player_2_score, player_high_score;

//player properties
internal void
simulate_player(float* p, float* dp, float ddp, float dt) {
	ddp -= *dp * 10.f;
	*p = *p + *dp * dt + ddp * dt * dt * .5f;
	*dp = *dp + ddp * dt;

	if (*p + player_half_size_y > arena_half_size_y) {
		*p = arena_half_size_y - player_half_size_y;
		*dp = 0;
	}
	else if (*p - player_half_size_y < -arena_half_size_y) {
		*p = -arena_half_size_y + player_half_size_y;
		*dp = 0;
	}
}

//random number generator
int getRandomNumber(int range) {
	// Generate a random number between 0 and RAND_MAX
	int randomValue = std::rand();

	// Calculate the maximum possible value without introducing bias
	int maxPossibleValue = RAND_MAX - (RAND_MAX % range);

	// Re-roll the random number if it's biased
	while (randomValue > maxPossibleValue) {
		randomValue = std::rand();
	}

	// Return a number within the specified range
	return randomValue % range;
}


//collision detection function
internal bool
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {
	return (p1x + hs1x > p2x - hs2x &&
		p1x - hs1x < p2x + hs2x &&
		p1y + hs1y > p2y - hs2y &&
		p1y + hs1y < p2y + hs2y);
}


//gamemode for game
enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY,
	GM_EXTRA_GAMEPLAY,
	GM_UI,
	GM_WINSCREEN,
	GM_LOSESCREEN,
	GM_JEOPARDY,
	GM_JEOPARDY_MP,
	GM_WINPLAYER,
	GM_MULTIPLAYER,
	GM_LOADING,
	GM_PAUSESP,
	GM_PAUSEMP,
	GM_LEADERBOARDS,
	GM_NAME,
	GM_DELETE,
	GM_PAUSEHS,
	GM_TUTORIAL_SINGLE,
	GM_TUTORIAL_SINGLE_MENU,
	GM_TUTORIAL_MULTIPLAYER_MENU,
	GM_TUTORIAL_MULTIPLAYER,
	GM_QUIT
};

Gamemode current_gamemode;
int hot_button;
int up_down = 0;
bool enemy_is_ai;
bool notTutorial;
int display;
int display2;
bool bluePowerup;
bool redPowerup;
bool gamestate;
bool question;
bool question2;
int random;
int random2;
int random3;
int countLong;
int speed;
int questionType;
int loadingTime;
bool pause;

//Answer structure
struct Answer {
	bool question;   // Text representing the answer option
	int isCorrect;     // Indicates whether the answer is correct or not

	// Constructor to initialize the Answer struct
	Answer(bool question, int isCorrect) : question(question), isCorrect(isCorrect) {}
};

Answer answer(false, 0);
Answer answer1(true, 0);
Answer answer2(true, 0);
Answer player1(true, 0);
Answer player2(true, 0);

int pointerX;
int pointerY;


float flashingIntervalColor1 = 0.3f; // Change to color2 every 0.3 seconds
float flashingIntervalColor2 = 0.8f; // Change to color1 every 0.8 seco
float elapsedTime = 0.0f; // Track time elapsed
bool useColor1 = true; // Flag to toggle between colors
internal void pressEnterAnimation(float dt) {

	// Define the colors for the flashing effect
	u32 color1 = 0xFFFF00; // Bright yellow
	u32 color2 = 0xCC9900; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime += dt;

	// Check if enough time has passed to change colors
	if (useColor1) {
		if (elapsedTime >= flashingIntervalColor1) {
			useColor1 = false;
			elapsedTime = 0.0f;
		}
	}
	else {
		if (elapsedTime >= flashingIntervalColor2) {
			useColor1 = true;
			elapsedTime = 0.0f;
		}
	}

	if (useColor1) {
		draw_text("PRESS [ENTER] TO CHOOSE", 27, -35, 0.4, color1);
	}
	else {
		draw_text("PRESS [ENTER] TO CHOOSE", 27, -35, 0.4, color2);
	}
}

internal void pressShiftAnimation(float dt) {

	// Define the colors for the flashing effect
	u32 color1 = 0xFFFF00; // Bright yellow
	u32 color2 = 0xCC9900; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime += dt;

	// Check if enough time has passed to change colors
	if (useColor1) {
		if (elapsedTime >= flashingIntervalColor1) {
			useColor1 = false;
			elapsedTime = 0.0f;
		}
	}
	else {
		if (elapsedTime >= flashingIntervalColor2) {
			useColor1 = true;
			elapsedTime = 0.0f;
		}
	}

	if (useColor1) {
		draw_text("PRESS [SHIFT] TO CHOOSE", 27, -35, 0.4, color1);
	}
	else {
		draw_text("PRESS [SHIFT] TO CHOOSE", 27, -35, 0.4, color2);
	}
}


float flashingIntervalColor13 = 0.3f; // Change to color2 every 0.3 seconds
float flashingIntervalColor3 = 0.8f; // Change to color1 every 0.8 seco
float elapsedTime1 = 0.0f; // Track time elapsed
bool useColor2 = true; // Flag to toggle between colors
internal void pressShiftQuestion(float dt) {
	// Define the colors for the flashing effect
	u32 color1 = 0x48FFFF; // Bright yellow
	u32 color2 = 0x008B8B; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime1 += dt;

	// Check if enough time has passed to change colors
	if (useColor2) {
		if (elapsedTime1 >= flashingIntervalColor13) {
			useColor2 = false;
			elapsedTime1 = 0.0f;
		}
	}
	else {
		if (elapsedTime1 >= flashingIntervalColor3) {
			useColor2 = true;
			elapsedTime1 = 0.0f;
		}
	}

	if (useColor2) {
		draw_text("[SHIFT]", -9, 8, 0.4, color1);
	}
	else {
		draw_text("[SHIFT]", -9, 8, 0.4, color2);
	}
}

internal void pressEnterQuestion(float dt) {
	// Define the colors for the flashing effect
	u32 color1 = 0x48FFFF; // Bright yellow
	u32 color2 = 0x008B8B; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime1 += dt;

	// Check if enough time has passed to change colors
	if (useColor2) {
		if (elapsedTime1 >= flashingIntervalColor13) {
			useColor2 = false;
			elapsedTime1 = 0.0f;
		}
	}
	else {
		if (elapsedTime1 >= flashingIntervalColor3) {
			useColor2 = true;
			elapsedTime1 = 0.0f;
		}
	}

	if (useColor2) {
		draw_text("[ENTER]", -10, 8, 0.4, color1);
	}
	else {
		draw_text("[ENTER]", -10, 8, 0.4, color2);
	}
}

float flashingIntervalColor4 = 0.3f; // Change to color2 every 0.3 seconds
float flashingIntervalColor5 = 0.8f; // Change to color1 every 0.8 seco
float elapsedTime2 = 0.0f; // Track time elapsed
bool useColor3 = true; // Flag to toggle between colors
internal void pressEscapeAnimation(float dt) {
	// Define the colors for the flashing effect
	u32 color1 = 0xFF0000; // Bright yellow
	u32 color2 = 0xB30000; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime2 += dt;

	// Check if enough time has passed to change colors
	if (useColor3) {
		if (elapsedTime2 >= flashingIntervalColor4) {
			useColor3 = false;
			elapsedTime2 = 0.0f;
		}
	}
	else {
		if (elapsedTime2 >= flashingIntervalColor5) {
			useColor3 = true;
			elapsedTime2 = 0.0f;
		}
	}

	if (useColor3) {
		draw_text("PRESS [ESC] TO RETURN TO MENU", -80, -35, 0.4, color1);
	}
	else {
		draw_text("PRESS [ESC] TO RETURN TO MENU", -80, -35, 0.4, color2);
	}
}

float flashingIntervalColor7 = 0.3f; // Change to color2 every 0.3 seconds
float flashingIntervalColor8 = 0.8f; // Change to color1 every 0.8 seco
float elapsedTime3 = 0.0f; // Track time elapsed
bool useColor4 = true; // Flag to toggle between colors

internal void pressQuicklyAnimation(float dt) {
	// Define the colors for the flashing effect
	u32 color1 = 0xFFFF00; // Bright yellow
	u32 color2 = 0xCC9900; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime3 += dt;

	// Check if enough time has passed to change colors
	if (useColor4) {
		if (elapsedTime3 >= flashingIntervalColor7) {
			useColor4 = false;
			elapsedTime3 = 0.0f;
		}
	}
	else {
		if (elapsedTime3 >= flashingIntervalColor8) {
			useColor4 = true;
			elapsedTime3 = 0.0f;
		}
	}

	if (useColor4) {
		draw_text("CHOOSE THE ANSWERS QUICKLY BEFORE THE BALL HITS YOUR GOAL", -80, 30, 0.3, color1);
	}
	else {
		draw_text("CHOOSE THE ANSWERS QUICKLY BEFORE THE BALL HITS YOUR GOAL", -80, 30, 0.3, color2);
	}
}
float flashingIntervalColor9 = 0.3f; // Change to color2 every 0.3 seconds
float flashingIntervalColor10 = 0.8f; // Change to color1 every 0.8 seco
float elapsedTime4 = 0.0f; // Track time elapsed
bool useColor5 = true; // Flag to toggle between colors
internal void pressP1QuicklyAnimation(float dt) {
	// Define the colors for the flashing effect
	u32 color1 = 0xFFFF00; // Bright yellow
	u32 color2 = 0xCC9900; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime4 += dt;

	// Check if enough time has passed to change colors
	if (useColor5) {
		if (elapsedTime4 >= flashingIntervalColor9) {
			useColor5 = false;
			elapsedTime4 = 0.0f;
		}
	}
	else {
		if (elapsedTime4 >= flashingIntervalColor10) {
			useColor5 = true;
			elapsedTime4 = 0.0f;
		}
	}

	if (useColor5) {
		draw_text("PLAYER ONE", -80, 30, 0.7, color1);
	}
	else {
		draw_text("PLAYER ONE", -80, 30, 0.7, color2);
	}
}

internal void pressP2QuicklyAnimation(float dt) {
	// Define the colors for the flashing effect
	u32 color1 = 0xFFFF00; // Bright yellow
	u32 color2 = 0xCC9900; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime4 += dt;

	// Check if enough time has passed to change colors
	if (useColor5) {
		if (elapsedTime4 >= flashingIntervalColor9) {
			useColor5 = false;
			elapsedTime4 = 0.0f;
		}
	}
	else {
		if (elapsedTime4 >= flashingIntervalColor10) {
			useColor5 = true;
			elapsedTime2 = 0.0f;
		}
	}

	if (useColor5) {
		draw_text("PLAYER TWO", -80, 30, 0.7, color1);
	}
	else {
		draw_text("PLAYER TWO", -80, 30, 0.7, color2);
	}
}
float flashingIntervalColor11 = 0.6f; // Change to color2 every 0.3 seconds
float flashingIntervalColor12 = 1.0f; // Change to color1 every 0.8 seco
float elapsedTime5 = 0.0f; // Track time elapsed
bool useColor6 = true; // Flag to toggle between colors
internal void WASD(float dt) {
	// Define the colors for the flashing effect
	u32 color1 = 0xAB47BC; // Bright yellow
	u32 color2 = 0x6A1B9A; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime5 += dt;

	// Check if enough time has passed to change colors
	if (useColor6) {
		if (elapsedTime5 >= flashingIntervalColor11) {
			useColor6 = false;
			elapsedTime5 = 0.0f;
		}
	}
	else {
		if (elapsedTime5 >= flashingIntervalColor12) {
			useColor6 = true;
			elapsedTime5 = 0.0f;
		}
	}

	if (useColor6) {
		draw_text("[W]", -58, -30, 0.4, color1);
		draw_text("PRESS [A][S][D] TO NAVIGATE", -80, -35, 0.4, color1);
	}
	else {
		draw_text("[W]", -58, -30, 0.4, color2);
		draw_text("PRESS [A][S][D] TO NAVIGATE", -80, -35, 0.4, color2);
	}
}

float flashingTutorial = 0.3f; // Change to color2 every 0.3 seconds
float flashingTutorial1 = 0.8f; // Change to color1 every 0.8 seco
float flashingTutorialTime = 0.0f; // Track time elapsed
bool useColorTutorial = true; // Flag to toggle between colors
internal void pressEscapeTutorialAnimation(float dt) {
	// Define the colors for the flashing effect
	u32 color1 = 0xFF0000; // Bright yellow
	u32 color2 = 0xB30000; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	flashingTutorialTime += dt;

	// Check if enough time has passed to change colors
	if (useColorTutorial) {
		if (flashingTutorialTime >= flashingTutorial) {
			useColorTutorial = false;
			flashingTutorialTime = 0.0f;
		}
	}
	else {
		if (flashingTutorialTime >= flashingTutorial1) {
			useColorTutorial = true;
			flashingTutorialTime = 0.0f;
		}
	}

	if (useColorTutorial) {
		draw_text("PRESS [ESC] TO END TUTORIAL", -80, 35, 0.4, color1);
	}
	else {
		draw_text("PRESS [ESC] TO END TUTORIAL", -80, 35, 0.4, color2);
	}
}

int gameplayTime = 0;
int multiplayerTime = 0;

//Tutorial menu when the player has not played the game
internal void tutorialSingleMenu(Input* input, float dt, int gamemode) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	draw_text("THIS IS YOUR FIRST TIME PLAYING", -55, 35, 0.6, 0xFFFF00);
	draw_text("DO YOU REQUIRE A TUTORIAL?", -42.5, 28, 0.6, 0xFFFF00);
	if (pressed(BUTTON_A)) {
		up_down = 0;
	}
	if (pressed(BUTTON_D)) {
		up_down = 1;
	}
	if (up_down == 0) {
		draw_text("YES", -50, -10, 1, 0x48FFFF);
		draw_text("NO", 42, -10, 1, 0x888888);
		draw_text("GO TO TUTORIAL", -60, -25, 0.5, 0x48FFFF);
		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_TUTORIAL_SINGLE;
		}
	}
	else {
		draw_text("YES", -50, -10, 1, 0x888888);
		draw_text("NO", 42, -10, 1, 0x48FFFF);
		draw_text("GO TO CATEGORY", 28, -25, 0.5, 0x48FFFF);
		if (pressed(BUTTON_ENTER) && gamemode == 1) {
			random = getRandomNumber(4);
			current_gamemode = GM_JEOPARDY;
		}
		else if (pressed(BUTTON_ENTER) && gamemode == 2) {
			random3 = getRandomNumber(4);
			current_gamemode = GM_JEOPARDY_MP;
		}
	}
}

//Jeopardy question categories
float elapsedTime7 = 0.0;
internal void jeopardy(Input* input, float dt, int gamemode) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	draw_text("CHOOSE YOUR QUESTION CATEGORY", -50, 35, 0.6, 0xFFFF00);
	pressEnterAnimation(dt);
	pressEscapeAnimation(dt);
	if (pressed(BUTTON_A) || pressed(BUTTON_D)) {
		pointerX = !pointerX;
	}
	if (pressed(BUTTON_W)) {
		pointerY = 0;
	}
	if (pressed(BUTTON_S)) {
		pointerY = 1;
	}

	if (pointerY == 0) {
		if (pointerX == 0) {
			draw_text("ALGORITHM", -80, 20, 1, 0x48FFFF);
			draw_lowercase_letter("java", 20, 20, 0.7f, 0x888888);
			draw_lowercase_letter("bits", -80, 10, 0.7f, 0x888888);
			draw_lowercase_letter("ml", 20, 10, 0.7f, 0x888888);
			draw_text("QUESTION RELATING TO THE ALGORITHM COURSE.", -80, -10, 0.4, 0xFFFFFF);
			draw_text("ASKS ABOUT COMPLEXITY OF DATA STRUCTURES.", -80, -20, 0.4, 0xFFFFFF);
			if (pressed(BUTTON_ENTER) && gamemode == 1) {
				elapsedTime7 = 0;
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 1;
				gameplayTime++;
			}
			else if (pressed(BUTTON_ENTER) && gamemode == 2) {
				random3 = getRandomNumber(4);
				current_gamemode = GM_MULTIPLAYER;
				questionType = 1;
				gameplayTime++;
			}
		}
		else {
			draw_lowercase_letter("algorithm", -80, 20, 0.7f, 0x888888);
			draw_text("JAVA", 20, 20, 1, 0x48FFFF);
			draw_lowercase_letter("bits", -80, 10, 0.7f, 0x888888);
			draw_lowercase_letter("ml", 20, 10, 0.7f, 0x888888);
			draw_text("QUESTION RELATING TO THE JAVA LANGUAGE.", -80, -10, 0.4, 0xFFFFFF);
			draw_text("ASKS ABOUT THE KNOWLEDGE OF JAVA.", -80, -20, 0.4, 0xFFFFFF);
			if (pressed(BUTTON_ENTER) && gamemode == 1) {
				elapsedTime7 = 0;
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 2;
				gameplayTime++;
			}
			else if (pressed(BUTTON_ENTER) && gamemode == 2) {
				random3 = getRandomNumber(4);
				current_gamemode = GM_MULTIPLAYER;
				questionType = 2;
				gameplayTime++;
			}
		}
	}
	else if (pointerY == 1) {
		if (pointerX == 0) {
			draw_lowercase_letter("algorithm", -80, 20, 0.7f, 0x888888);
			draw_lowercase_letter("java", 20, 20, 0.7f, 0x888888);
			draw_text("BITS", -80, 10, 1, 0x48FFFF);
			draw_lowercase_letter("ml", 20, 10, 0.7f, 0x888888);
			draw_text("QUESTION RELATING TO THE BUILING IT SYSTEMS COURSE.", -80, -10, 0.4, 0xFFFFFF);
			draw_text("ASKS ABOUT THE SKILLS NEEDED FOR WORKING IN A GROUP.", -80, -20, 0.4, 0xFFFFFF);
			if (pressed(BUTTON_ENTER) && gamemode == 1) {
				elapsedTime7 = 0;
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 3;
				gameplayTime++;
			}
			else if (pressed(BUTTON_ENTER) && gamemode == 2) {
				random3 = getRandomNumber(4);
				current_gamemode = GM_MULTIPLAYER;
				questionType = 3;
				gameplayTime++;
			}
		}
		else {
			draw_lowercase_letter("algorithm", -80, 20, 0.7f, 0x888888);
			draw_lowercase_letter("java", 20, 20, 0.7f, 0x888888);
			draw_lowercase_letter("bits", -80, 10, 0.7f, 0x888888);
			draw_text("ML", 20, 10, 1, 0x48FFFF);
			draw_text("QUESTION RELATING TO MACHINE LEARNING.", -80, -10, 0.4, 0xFFFFFF);
			draw_text("ASKS ABOUT THE MACHINE LEARNING CONCEPTS.", -80, -20, 0.4, 0xFFFFFF);
			if (pressed(BUTTON_ENTER) && gamemode == 1) {
				elapsedTime7 = 0;
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 4;
				gameplayTime++;
			}
			else if (pressed(BUTTON_ENTER) && gamemode == 2) {
				random3 = getRandomNumber(4);
				current_gamemode = GM_MULTIPLAYER;
				questionType = 4;
				gameplayTime++;
			}
		}
	}
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_UI;
	}
}

//Question hints
const char* box[] = {
	"00000000",
	"0      0",
	"0      0",
	"0      0",
	"0      0",
	"0      0",
	"00000000",
};
//

//Power ups
const char* speedBoost[] = {
	"000000000000",
	"0          0",
	"0   1  1   0",
	"0    1  1  0",
	"0   1  1   0",
	"0          0",
	"000000000000",
};

const char* tallerBoost[] = {
  "000000000",
  "0       0",
  "0   1   0",
  "0  111  0",
  "0 11111 0",
  "0   1   0",
  "000000000",
};

const char* shorterDebuff[] = {
  "000000000",
  "0   1   0",
  "0 11111 0",
  "0  111  0",
  "0   1   0",
  "0       0",
  "000000000",
};

const char* extraWallDebuff[] = {
	"000000000000",
	"0   1  1   0",
	"0   1  1   0",
	"0   1  1   0",
	"0   1  1   0",
	"0   1  1   0",
	"000000000000",
};
//

float player_Answers = 0;
internal void testQuestion(Input* input, int questionIndex, float dt) {
	pressShiftAnimation(dt);
	pressShiftQuestion(dt);
	pressQuicklyAnimation(dt);
	std::map<int, Question> questions;
	if (questionType == 1) {
		questions = questionAlgo;
	}
	else if (questionType == 2) {
		questions = questionJava;
	}
	else if (questionType == 3) {
		questions = questionBITS;
	}
	else {
		questions = questionML;
	}

	// Seed the random number generator with the current time
	auto it = questions.find(questionIndex);

	if (it != questions.end()) {
		const Question& currentQuestion = it->second;

		// Extract question text, answer options, and correct answer from currentQuestion
		const char* questionText = currentQuestion.questionText;
		const std::vector<const char*>& answerOptions = currentQuestion.answerOptions;
		const char* correctAnswer = currentQuestion.correctAnswer;

		// Display the question
		draw_text(questionText, -80, 20, 0.6, 0xFFFF00);

		if (pressed(BUTTON_A)) {
			display = 1;
		}

		if (pressed(BUTTON_D)) {
			display = 2;
		}

		if (pressed(BUTTON_S)) {
			display = 3;
		}
		if (display == 1) {
			draw_text("A", -46, 8.8, 0.5, 0x48FFFF);
			draw_text(answerOptions[0], -58, 0, 0.45, 0x48FFFF);
			render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
			draw_text("D", 40.6, 8.8, 0.5, 0x888888);
			draw_text(answerOptions[1], 30, 0, 0.45, 0x888888);
			render_ascii_art(box, 29, 7, 38, 10, 1, 0x888888, 0x888888, 0x888888);
			draw_text("S", -3, -14, 0.5, 0x888888);
			draw_text(answerOptions[2], -12, -23.5, 0.45, 0x888888);
			render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
		}
		else if (display == 2) {
			draw_text("A", -46, 8.8, 0.5, 0x888888);
			draw_text(answerOptions[0], -58, 0, 0.45, 0x888888);
			render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x888888, 0x888888, 0x888888);
			draw_text("D", 40.6, 8.8, 0.5, 0x48FFFF);
			draw_text(answerOptions[1], 30, 0, 0.45, 0x48FFFF);
			render_ascii_art(box, 29, 7, 38, 10, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
			draw_text("S", -3, -14, 0.5, 0x888888);
			draw_text(answerOptions[2], -12, -23.5, 0.45, 0x888888);
			render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
		}
		else {
			draw_text("A", -46, 8.8, 0.5, 0x888888);
			draw_text(answerOptions[0], -58, 0, 0.45, 0x888888);
			render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x888888, 0x888888, 0x888888);
			draw_text("D", 40.6, 8.8, 0.5, 0x888888);
			draw_text(answerOptions[1], 30, 0, 0.45, 0x888888);
			render_ascii_art(box, 9, 7, 38, 10, 1, 0x888888, 0x888888, 0x888888);
			draw_text("S", -3, -14, 0.5, 0x48FFFF);
			draw_text(answerOptions[2], -12, -23.5, 0.45, 0x48FFFF);
			render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
		}

		if (pressed(BUTTON_SHIFT)) {
			// Check if the player selected the correct answer
			const char* playerChoice;
			if (display == 1) {
				playerChoice = answerOptions[0];
			}
			else if (display == 2) {
				playerChoice = answerOptions[1];
			}
			else {
				playerChoice = answerOptions[2];
			}

			if (strcmp(playerChoice, correctAnswer) == 0) {
				player_Answers++;
				random = getRandomNumber(4);
				random2 = getRandomNumber(100);
				// Player answered correctly
				answer1.isCorrect = 1;
				answer1.question = true;
				//question = true;f
			}
			else {
				player_Answers++;
				random = getRandomNumber(4);
				random2 = getRandomNumber(100);
				// Player answered incorrectly
				answer1.isCorrect = 2;
				answer1.question = true;
			}
		}
	}
}

float player_1_Answers = 0;
float player_2_Answers = 0;
internal void testQuestionMultiplayer(Input* input, int questionIndex, int playerType, float dt) {
	pressShiftAnimation(dt);
	pressShiftQuestion(dt);
	if (playerType == 1) {
		pressP1QuicklyAnimation(dt);
	}
	else {
		pressP2QuicklyAnimation(dt);
	}
	std::map<int, Question> questions;
	if (questionType == 1) {
		questions = questionAlgo;
	}
	else if (questionType == 2) {
		questions = questionJava;
	}
	else if (questionType == 3) {
		questions = questionBITS;
	}
	else {
		questions = questionML;
	}
	// Seed the random number generator with the current time
	auto it = questions.find(questionIndex);

	if (it != questions.end()) {
		const Question& currentQuestion = it->second;

		// Extract question text, answer options, and correct answer from currentQuestion
		const char* questionText = currentQuestion.questionText;
		const std::vector<const char*>& answerOptions = currentQuestion.answerOptions;
		const char* correctAnswer = currentQuestion.correctAnswer;

		// Display the question
		draw_text(questionText, -80, 20, 0.4, 0xFFFF00);
		if (playerType == 2) {
			if (pressed(BUTTON_A)) {
				display = 1;
			}

			if (pressed(BUTTON_D)) {
				display = 2;
			}

			if (pressed(BUTTON_S)) {
				display = 3;
			}
			if (display == 1) {
				draw_text("A", -46, 8.8, 0.5, 0x48FFFF);
				draw_text(answerOptions[0], -58, 0, 0.45, 0x48FFFF);
				render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
				draw_text("D", 40.6, 8.8, 0.5, 0x888888);
				draw_text(answerOptions[1], 30, 0, 0.45, 0x888888);
				render_ascii_art(box, 29, 7, 38, 10, 1, 0x888888, 0x888888, 0x888888);
				draw_text("S", -3, -14, 0.5, 0x888888);
				draw_text(answerOptions[2], -12, -23.5, 0.45, 0x888888);
				render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
			}
			else if (display == 2) {
				draw_text("A", -46, 8.8, 0.5, 0x888888);
				draw_text(answerOptions[0], -58, 0, 0.45, 0x888888);
				render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x888888, 0x888888, 0x888888);
				draw_text("D", 40.6, 8.8, 0.5, 0x48FFFF);
				draw_text(answerOptions[1], 30, 0, 0.45, 0x48FFFF);
				render_ascii_art(box, 29, 7, 38, 10, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
				draw_text("S", -3, -14, 0.5, 0x888888);
				draw_text(answerOptions[2], -12, -23.5, 0.45, 0x888888);
				render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
			}
			else {
				draw_text("A", -46, 8.8, 0.5, 0x888888);
				draw_text(answerOptions[0], -58, 0, 0.45, 0x888888);
				render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x888888, 0x888888, 0x888888);
				draw_text("D", 40.6, 8.8, 0.5, 0x888888);
				draw_text(answerOptions[1], 30, 0, 0.45, 0x888888);
				render_ascii_art(box, 9, 7, 38, 10, 1, 0x888888, 0x888888, 0x888888);
				draw_text("S", -3, -14, 0.5, 0x48FFFF);
				draw_text(answerOptions[2], -12, -23.5, 0.45, 0x48FFFF);
				render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
			}
		}
		else if (playerType == 1) {
			if (pressed(BUTTON_LEFT)) {
				display = 1;
			}

			if (pressed(BUTTON_RIGHT)) {
				display = 2;
			}

			if (pressed(BUTTON_DOWN)) {
				display = 3;
			}
			if (display == 1) {
				draw_text("(", -46.5, 9, 0.5, 0x48FFFF);
				draw_text(answerOptions[0], -58, 0, 0.45, 0x48FFFF);
				render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
				draw_text(")", 40, 9, 0.5, 0x888888);
				draw_text(answerOptions[1], 30, 0, 0.45, 0x888888);
				render_ascii_art(box, 29, 7, 38, 10, 1, 0x888888, 0x888888, 0x888888);
				draw_text("<", -3.7, -14, 0.5, 0x888888);
				draw_text(answerOptions[2], -12, -23.5, 0.45, 0x888888);
				render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
			}
			else if (display == 2) {
				draw_text("(", -46.5, 9, 0.5, 0x888888);
				draw_text(answerOptions[0], -58, 0, 0.45, 0x888888);
				render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x888888, 0x888888, 0x888888);
				draw_text(")", 40, 9, 0.5, 0x48FFFF);
				draw_text(answerOptions[1], 30, 0, 0.45, 0x48FFFF);
				render_ascii_art(box, 29, 7, 38, 10, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
				draw_text("<", -3.7, -14, 0.5, 0x888888);
				draw_text(answerOptions[2], -12, -23.5, 0.45, 0x888888);
				render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
			}
			else {
				draw_text("(", -46.5, 9, 0.5, 0x888888);
				draw_text(answerOptions[0], -58, 0, 0.45, 0x888888);
				render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x888888, 0x888888, 0x888888);
				draw_text(")", 40, 9, 0.5, 0x888888);
				draw_text(answerOptions[1], 30, 0, 0.45, 0x888888);
				render_ascii_art(box, 9, 7, 38, 10, 1, 0x888888, 0x888888, 0x888888);
				draw_text("<", -3.7, -14, 0.5, 0x48FFFF);
				draw_text(answerOptions[2], -12, -23.5, 0.45, 0x48FFFF);
				render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
			}
		}
		if (pressed(BUTTON_SHIFT)) {
			// Check if the player selected the correct answer
			const char* playerChoice;
			if (display == 1) {
				playerChoice = answerOptions[0];
			}
			else if (display == 2) {
				playerChoice = answerOptions[1];
			}
			else {
				playerChoice = answerOptions[2];
			}

			if (strcmp(playerChoice, correctAnswer) == 0 && playerType == 1) {
				random3 = getRandomNumber(3);
				random2 = getRandomNumber(100);
				// Player answered correctly
				player1.question = true;
				player_1_Answers++;
				player1.isCorrect = 1;
				//question = true;f
			}
			else if (!(strcmp(playerChoice, correctAnswer) == 0) && playerType == 1) {
				random3 = getRandomNumber(3);
				random2 = getRandomNumber(100);
				player1.question = true;
				player_1_Answers++;
				// Player answered incorrectly
				player1.isCorrect = 2;
			}
			else if (strcmp(playerChoice, correctAnswer) == 0 && playerType == 2) {
				random3 = getRandomNumber(3);
				random2 = getRandomNumber(100);
				player2.question = true;
				player_2_Answers++;
				player2.isCorrect = 1;
			}
			else if (!(strcmp(playerChoice, correctAnswer) == 0) && playerType == 2) {
				random3 = getRandomNumber(3);
				random2 = getRandomNumber(100);
				player2.question = true;
				player_2_Answers++;
				player2.isCorrect = 2;
			}
		}
	}
}
u32 ballColor = 0xFF49B3;
u32 slideColor = 0x6666FF * 2;

float intervalAppear = 3.0;
u32 lower;
u32 upper;
float tracking_speed_factor;
float overall_speed;

int player_Tutorial = 0;
internal void testQuestionTutorial(Input* input, float dt) {
	pressShiftAnimation(dt);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	pressShiftQuestion(dt);
	// Display the question
	draw_text("THIS IS A DEFAULT QUESTION", -80, 20, 0.6, 0xFFFF00);
	draw_text("IN THE REAL GAME A SLOW MOVING BALL WILL ATTACK YOUR GOAL", -85, 35, 0.5, 0xFFFF00);
	draw_text("YOU WILL HAVE TO CHOOSE YOUR ANSWER QUICKLY", -85, 28, 0.5, 0xFFFF00);
	if (pressed(BUTTON_A)) {
		display = 1;
	}

	if (pressed(BUTTON_D)) {
		display = 2;
	}

	if (pressed(BUTTON_S)) {
		display = 3;
	}
	if (display == 1) {
		draw_text("A", -46, 8.8, 0.5, 0x48FFFF);
		draw_text("RIGHT ANSWER", -58, 0, 0.45, 0x48FFFF);
		render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
		draw_text("D", 40.6, 8.8, 0.5, 0x888888);
		draw_text("WRONG ANSWER", 30, 0, 0.45, 0x888888);
		render_ascii_art(box, 29, 7, 38, 10, 1, 0x888888, 0x888888, 0x888888);
		draw_text("S", -3, -14, 0.5, 0x888888);
		draw_text("WRONG ANSWER", -12, -23.5, 0.45, 0x888888);
		render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
	}
	else if (display == 2) {
		draw_text("A", -46, 8.8, 0.5, 0x888888);
		draw_text("RIGHT ANSWER", -58, 0, 0.45, 0x888888);
		render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x888888, 0x888888, 0x888888);
		draw_text("D", 40.6, 8.8, 0.5, 0x48FFFF);
		draw_text("WRONG ANSWER", 30, 0, 0.45, 0x48FFFF);
		render_ascii_art(box, 29, 7, 38, 10, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
		draw_text("S", -3, -14, 0.5, 0x888888);
		draw_text("WRONG ANSWER", -12, -23.5, 0.45, 0x888888);
		render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
	}
	else {
		draw_text("A", -46, 8.8, 0.5, 0x888888);
		draw_text("RIGHT ANSWER", -58, 0, 0.45, 0x888888);
		render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x888888, 0x888888, 0x888888);
		draw_text("D", 40.6, 8.8, 0.5, 0x888888);
		draw_text("WRONG ANSWER", 30, 0, 0.45, 0x888888);
		render_ascii_art(box, 9, 7, 38, 10, 1, 0x888888, 0x888888, 0x888888);
		draw_text("S", -3, -14, 0.5, 0x48FFFF);
		draw_text("WRONG ANSWER", -12, -23.5, 0.45, 0x48FFFF);
		render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
	}

	if (pressed(BUTTON_SHIFT)) {
		// Check if the player selected the correct answer
		bool playerChoice;
		if (display == 1) {
			playerChoice = true;
		}
		else if (display == 2) {
			playerChoice = false;
		}
		else {
			playerChoice = false;
		}

		if (playerChoice == true) {
			player_Answers++;
			// Player answered correctly
			answer2.isCorrect = 1;
			answer2.question = true;
			player_Tutorial = 0;
			//question = true;f
		}
		else {
			player_Answers++;
			// Player answered incorrectly
			answer2.isCorrect = 2;
			answer2.question = true;
			player_Tutorial = 0;
		}
	}
}

float tutorialTime = 0.0f;
int movementTime = 10.0f;
int ballTime = 20.0f;
int player_Tutorial_Score = 0;

internal void tutorialSingle(Input* input, float dt) {
	float player_2_half_size_x = 2.5, player_2_half_size_y = 12;
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	pressEscapeTutorialAnimation(dt);
	if (pressed(BUTTON_ESCAPE)) {
		gameplayTime++;
		random = getRandomNumber(4);
		current_gamemode = GM_UI;
		playMusic1 = false;


	}
	float player_2_ddp = 0.0f;
	if (answer2.isCorrect == 1) {
		if (is_down(BUTTON_W)) player_2_ddp += 5000;
		if (is_down(BUTTON_S)) player_2_ddp -= 5000;
		draw_text("YOU PICKED THE CORRECT ANSWER", -80, 20, 0.6, 0xFFFF00);
		draw_text("YOU GOT A POWERUP", -80, 0, 0.6, 0xFFFF00);
		draw_text("YOU NOW MOVE FASTER", -80, -20, 0.6, 0xFFFF00);
		slideColor = 0xb10000;
	}
	else {
		if (is_down(BUTTON_W)) player_2_ddp += 2000;
		if (is_down(BUTTON_S)) player_2_ddp -= 2000;
	}
	simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);
	if (answer2.isCorrect == 2) {
		player_2_half_size_y = 10;
		draw_text("YOU PICKED THE WRONG ANSWER", -80, 20, 0.6, 0xFFFF00);
		draw_text("YOU GOT A POWERDOWN", -80, 0, 0.6, 0xFFFF00);
		draw_text("YOU ARE SMOL NOW", -80, -20, 0.6, 0xFFFF00);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0x964B00);
	}
	else {
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0x964B00);
	}
	tutorialTime += dt;
	if (tutorialTime <= movementTime) {
		draw_text(">", -79.5, 30, 1, 0xFFFF00);
		draw_text("W", -79, 20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, -82.5, 22, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("S", -79, -20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, -82.5, -18, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("<", -79.5, -30, 1, 0xFFFF00);
		draw_text("USE W AND S TO MOVE AROUND", -40, 0, 0.6, 0xFFFF00);
	}
	else {
		ball_p_x += ball_dp_x * dt;
		ball_p_y += ball_dp_y * dt;
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, ballColor);

		if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -80, player_2_p, player_2_half_size_x, player_2_half_size_y)
			) {
			ball_p_x = -80 + player_half_size_x + ball_half_size;
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player_2_p) * 2 + player_2_dp * .75f;
			ballColor = 0xFF6347;
		}


		if (ball_p_y + ball_half_size > arena_half_size_y) {
			ball_p_y = arena_half_size_y - ball_half_size;
			ball_dp_y *= -1;
			ballColor = 0xFFFF00;
			upper = 0x008000;
		}
		else if (ball_p_y - ball_half_size < -arena_half_size_y) {
			ball_p_y = -arena_half_size_y + ball_half_size;
			ball_dp_y *= -1;
			ballColor = 0xFFFF00;
			lower = 0x008000;
		}
		else {
			lower = 0x464651;
			upper = 0x464651;
		}
		if (tutorialTime <= ballTime) {
			draw_text("HIT THE BALL TO MAKE IT MOVE", -80, 20, 0.6, 0xFFFF00);
			draw_text("AND SCORE INTO THE OPPONENT GOAL", -80, 0, 0.6, 0xFFFF00);

			if (ball_p_x + ball_half_size > arena_half_size_x) {
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;
				ballColor = 0xFF49B3;
			}
		}
		else {
			if (ball_p_x + ball_half_size > arena_half_size_x) {
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;
				player_Tutorial_Score++;
				ballColor = 0xFF49B3;
				player_Tutorial++;
			}
		}
		if (ball_p_x - ball_half_size < -arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			ballColor = 0xFF49B3;
		}
		if (tutorialTime > ballTime) {
			draw_number(player_Tutorial_Score, -10, 40, 1.f, 0xbbffbb);
			if (player_Tutorial_Score < 5) {
				draw_text("TRY TO SCORE FIVE POINTS TO GET A QUESTION", -80, 0, 0.6, 0xFFFF00);
			}
			if (player_Tutorial == 5) {
				ball_p_x = 0;
				ball_p_y = 0;
				testQuestionTutorial(input, dt);
			}
		}
	}
}

internal void gameplay(Input* input, float dt) {

	float player_2_half_size_x = 2.5, player_2_half_size_y = 12;
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders_special(arena_half_size_x, arena_half_size_y, 0x464651, lower, upper);
	elapsedTime7 += dt;
	if (elapsedTime7 <= intervalAppear) {
		draw_text("PRESS [ESC] TO OPEN MENU", -80, 35, 0.5, 0xFFFF00);
		draw_text(">", -79.5, 30, 1, 0xFFFF00);
		draw_text("W", -79, 20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, -82.5, 22, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("S", -79, -20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, -82.5, -18, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("<", -79.5, -30, 1, 0xFFFF00);
	}
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_PAUSESP;
	}
	bool slowMo = false;
	if (player_Answers > 5) {
		player_Answers = 0;
	}
	answer1.question = true;
	if (player_Answers == 5) {
		slowMo = true;
		answer1.question = false;
	}
	float player_1_ddp = 0.f;
	if (player_1_score >= 10) {
		tracking_speed_factor = 250;
		overall_speed = 1700;
	}
	else {
		tracking_speed_factor = 100;
		overall_speed = 1300;
	}
	player_1_ddp = (ball_p_y - player_1_p) * tracking_speed_factor;
	if (player_1_ddp > overall_speed) player_1_ddp = overall_speed;
	if (player_1_ddp < -overall_speed) player_1_ddp = -overall_speed;
	float player_3_ddp = 0;
	float player_2_ddp = 0.f;
	if (slowMo) {
		player_2_p = 0;
		if (is_down(BUTTON_W)) player_2_ddp += 0;
		if (is_down(BUTTON_S)) player_2_ddp -= 0;
	}
	if (answer1.isCorrect == 1 && random2 >= 50) {
		if (is_down(BUTTON_W)) player_2_ddp += 5000;
		if (is_down(BUTTON_S)) player_2_ddp -= 5000;
		render_ascii_art(speedBoost, 9, 7, -80, -35, 1, 0x888888, 0x888888, 0x888888);
		draw_text("FASTER BOOST", -65, -36.5, 0.5, 0x888888);
		slideColor = 0xb10000;
	}
	else {
		if (is_down(BUTTON_W)) player_2_ddp += 2000;
		if (is_down(BUTTON_S)) player_2_ddp -= 2000;
	}

	simulate_player(&player_1_p, &player_1_dp, player_1_ddp, dt);
	simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);
	if (slowMo == true) {
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0x000000);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0x000000);
	}
	else if (answer1.isCorrect == 1 && random2 < 50) {
		player_2_half_size_y = 16;
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xD6178F);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0x964B00);
		render_ascii_art(tallerBoost, 9, 7, -80, -35, 1, 0x888888, 0x888888, 0x888888);
		draw_text("TALLER BOOST", -65, -36.5, 0.5, 0x888888);
	}
	else if (answer1.isCorrect == 2 && random2 >= 50) {
		player_2_half_size_y = 10;
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xD6178F);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xADD8E6);
		render_ascii_art(shorterDebuff, 9, 7, -80, -35, 1, 0x888888, 0x888888, 0x888888);
		draw_text("SHORTER DEBUFF", -65, -36.5, 0.5, 0x888888);
	}
	else {
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xD6178F);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xCCCDFF);
	}
	if (player_Answers == 5) {
		testQuestion(input, random, dt);
		draw_arena_borders_special(arena_half_size_x, arena_half_size_y, 0x383841, 0x383841, 0x383841);
	}
	else {
		draw_arena_borders_special(arena_half_size_x, arena_half_size_y, 0x383841, 0x383841, 0x383841);
	}
	if (slowMo) {
		ball_p_x += ball_dp_x * 0.1f * dt;
		ball_p_y += ball_dp_y * 0.1f * dt;
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);
		if (ball_p_x - ball_half_size < -arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			answer1.question = true;
			slowMo = false;
			player_2_score++;
			player_Answers++;
		}
	}
	else {
		ball_p_x += ball_dp_x * dt;
		ball_p_y += ball_dp_y * dt;
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, ballColor);


		if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 80, player_1_p, player_half_size_x, player_half_size_y)
			) {
			ball_p_x = 80 - player_half_size_x - ball_half_size;
			ball_dp_x *= -1;
			//ball_dp_y = player_1_dp *.75f; 
			ball_dp_y = (ball_p_y - player_1_p) * 2 + player_1_dp * .75f;
			ballColor = 0x00FFFF;
		}

		if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -80, player_2_p, player_2_half_size_x, player_2_half_size_y)
			) {
			ball_p_x = -80 + player_half_size_x + ball_half_size;
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player_2_p) * 2 + player_2_dp * .75f;
			ballColor = 0xFF6347;
		}


		if (ball_p_y + ball_half_size > arena_half_size_y) {
			ball_p_y = arena_half_size_y - ball_half_size;
			ball_dp_y *= -1;
			ballColor = 0xFFFF00;
			upper = 0x008000;
		}
		else if (ball_p_y - ball_half_size < -arena_half_size_y) {
			ball_p_y = -arena_half_size_y + ball_half_size;
			ball_dp_y *= -1;
			ballColor = 0xFFFF00;
			lower = 0x008000;
		}
		else {
			lower = 0x464651;
			upper = 0x464651;
		}
		if (ball_p_x + ball_half_size > arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			player_1_score++;
			ballColor = 0xFF49B3;
			player_Answers++;
		}
		if (ball_p_x - ball_half_size < -arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			player_2_score++;
			ballColor = 0xFF49B3;
		}

	}

	//HEALTH

	//float at_x = -80;
	//for (int i = 0; i < player_1_score; i++) {
	//	draw_rect(at_x, 47.f, 1.f, 1.f, 0xaaaaaa);
	//	at_x += 2.5f;
	//}
	//at_x = 80;
	//for (int i = 0; i < player_2_score; i++) {
	//	draw_rect(at_x, 47.f, 1.f, 1.f, 0xaaaaaa);
	//	at_x -= 2.5f;
	//}

	draw_number(player_1_score, -10, 40, 1.f, 0xbbffbb);
	draw_number(player_2_score, 10, 40, 1.f, 0xbbffbb);
	// Inside your gameplay function
	// Detect collision with player 3 on all sides
	if (answer1.question == true && answer1.isCorrect == 2 && random2 < 50) {
		static bool movingUp = true;
		const float movementSpeed = 20.0f; // Adjust the speed of movement
		const float maxUpPosition = 20.0f; // Adjust the maximum up position
		const float minDownPosition = -20.0f; // Adjust the minimum down position
		if (movingUp) {
			player_3_p += movementSpeed * dt; // Move up
			if (player_3_p >= maxUpPosition) {
				player_3_p = maxUpPosition; // Limit at the maximum up position
				movingUp = false; // Change direction
			}
		}
		else {
			player_3_p -= movementSpeed * dt; // Move down
			if (player_3_p <= minDownPosition) {
				player_3_p = minDownPosition; // Limit at the minimum down position
				movingUp = true; // Change direction
			}
		}
		simulate_player(&player_3_p, &player_3_dp, player_3_ddp, dt);
		if ((ball_p_x + ball_half_size > 30 - player_half_size_x) &&
			(ball_p_x - ball_half_size < 30 + player_half_size_x) &&
			(ball_p_y + ball_half_size > player_3_p - 17) &&
			(ball_p_y - ball_half_size < player_3_p + 17)) {
			// Handle collision with player 3 here
			if (ball_p_x < 30) {
				ball_p_x = 30 - player_half_size_x - ball_half_size;
			}
			else {
				ball_p_x = 30 + player_half_size_x + ball_half_size;
			}
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player_3_p) * 2 + player_3_dp * 0.75f;
			ballColor = 0x808080;
		}

		draw_rect(30, player_3_p, player_half_size_x, 17, 0x8D155E);
		render_ascii_art(extraWallDebuff, 9, 7, -80, -35, 1, 0x888888, 0x888888, 0x888888);
		draw_text("EXTRA WALL", -65, -36.5, 0.5, 0x888888);
	}
}

float elapsedTime6 = 0.0;
float intervalAppear2 = 3.0;
internal void multiplayer(Input* input, float dt) {
	bool slowMo1 = false;
	bool slowMo2 = false;
	float player_1_half_size_x = 2.5, player_1_half_size_y = 12;
	float player_2_half_size_x = 2.5, player_2_half_size_y = 12;
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_PAUSEMP;
	}
	player1.question = true;
	player2.question = true;
	if (player_1_Answers > 5) {
		player_1_Answers = 0;
	}
	if (player_2_Answers > 5) {
		player_2_Answers = 0;
	}
	if (player_1_Answers == 5) {
		slowMo1 = true;
		player1.isCorrect = 0;
		player2.isCorrect = 0;

		player1.question = false;
	}
	else if (player_2_Answers == 5) {
		slowMo2 = true;
		player1.isCorrect = 0;
		player2.isCorrect = 0;
		player2.question = false;
	}
	float player_1_ddp = 0.f;
	elapsedTime6 += dt;
	if (elapsedTime6 <= intervalAppear2) {
		draw_text("PRESS [ESC] TO OPEN MENU", -80, 35, 0.5, 0xFFFF00);
		draw_text(">", -79.5, 30, 1, 0xFFFF00);
		draw_text("W", -79, 20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, -82.5, 22, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("S", -79, -20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, -82.5, -18, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("<", -79.5, -30, 1, 0xFFFF00);
		draw_text(">", 74.5, 30, 1, 0xFFFF00);
		draw_text(">", 74, 20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, 71.5, 22, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("<", 74, -20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, 71.5, -18, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("<", 74.5, -30, 1, 0xFFFF00);
	}
	if (slowMo1 == true || slowMo2 == true) {
		if (is_down(BUTTON_UP)) player_1_ddp += 0;
		if (is_down(BUTTON_DOWN)) player_1_ddp -= 0;
	}
	else if (player1.isCorrect == 1 && random2 <= 50) {
		if (is_down(BUTTON_UP)) player_1_ddp += 5000;
		if (is_down(BUTTON_DOWN)) player_1_ddp -= 5000;
		render_ascii_art(speedBoost, 9, 7, 70, -25, 1, 0x888888, 0x888888, 0x888888);
		draw_text("FASTER BOOST", 30, -26.5, 0.5, 0x888888);
	}
	else {
		if (is_down(BUTTON_UP)) player_1_ddp += 2000;
		if (is_down(BUTTON_DOWN)) player_1_ddp -= 2000;
	}
	float player_3_ddp = 0;
	float player_4_ddp = 0;
	float player_2_ddp = 0.f;
	if (slowMo1 == true || slowMo2 == true) {
		if (is_down(BUTTON_W)) player_2_ddp += 0;
		if (is_down(BUTTON_S)) player_2_ddp -= 0;
	}
	else if (player2.isCorrect == 1 && random2 <= 50) {
		if (is_down(BUTTON_W)) player_2_ddp += 5000;
		if (is_down(BUTTON_S)) player_2_ddp -= 5000;
		render_ascii_art(speedBoost, 9, 7, -80, -25, 1, 0x888888, 0x888888, 0x888888);
		draw_text("FASTER BOOST", -65, -26.5, 0.5, 0x888888);
	}
	else {
		if (is_down(BUTTON_W)) player_2_ddp += 2000;
		if (is_down(BUTTON_S)) player_2_ddp -= 2000;
	}

	simulate_player(&player_1_p, &player_1_dp, player_1_ddp, dt);
	simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);
	if (player1.question == false || player2.question == false) {
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0x000000);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0x000000);
	}
	else if (player2.isCorrect == 1 && random2 > 50) {
		player_2_half_size_y = 16;
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xff0000);
		draw_rect(80, player_1_p, player_1_half_size_x, player_1_half_size_y, 0xff0000);
		render_ascii_art(tallerBoost, 9, 7, -80, -15, 1, 0x888888, 0x888888, 0x888888);
		draw_text("TALLER BOOST", -65, -16.5, 0.5, 0x888888);
	}
	else if (player2.isCorrect == 2 && random2 > 50) {
		player_2_half_size_y = 8;
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xff0000);
		draw_rect(80, player_1_p, player_1_half_size_x, player_1_half_size_y, 0xff0000);
		render_ascii_art(shorterDebuff, 9, 7, -80, -15, 1, 0x888888, 0x888888, 0x888888);
		draw_text("SHORTER DEBUFF", -65, -16.5, 0.5, 0x888888);
	}
	else if (player1.isCorrect == 1 && random2 > 50) {
		player_1_half_size_y = 16;
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xff0000);
		draw_rect(80, player_1_p, player_1_half_size_x, player_1_half_size_y, 0xff0000);
		render_ascii_art(tallerBoost, 9, 7, 70, -15, 1, 0x888888, 0x888888, 0x888888);
		draw_text("TALLER BOOST", 30, -16.5, 0.5, 0x888888);
	}
	else if (player1.isCorrect == 2 && random2 > 50) {
		player_1_half_size_y = 8;
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xff0000);
		draw_rect(80, player_1_p, player_1_half_size_x, player_1_half_size_y, 0xff0000);
		render_ascii_art(shorterDebuff, 9, 7, 70, -15, 1, 0x888888, 0x888888, 0x888888);
		draw_text("SHORTER DEBUFF", 25, -16.5, 0.5, 0x888888);
	}
	else {
		draw_rect(80, player_1_p, player_1_half_size_x, player_1_half_size_y, 0x6666FF * 2);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xD6178F);
	}
	if (player1.question == false) {
		testQuestionMultiplayer(input, random3, 1, dt);
	}
	else if (player2.question == false) {
		testQuestionMultiplayer(input, random3, 2, dt);
	}
	if (slowMo2) {
		ball_p_x += ball_dp_x * 0.1f * dt;
		ball_p_y += ball_dp_y * 0.1f * dt;
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);
		if (ball_p_x - ball_half_size < -arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			player1.question = true;
			slowMo1 = false;
			player_1_score++;
			player_2_Answers++;
		}
	}
	else if (slowMo1) {
		ball_p_x += ball_dp_x * 0.1f * dt;
		ball_p_y += ball_dp_y * 0.1f * dt;
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);
		if (ball_p_x + ball_half_size > arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			player2.question = true;
			slowMo2 = false;
			player_2_score++;
			player_1_Answers++;
		}
	}
	else {
		ball_p_x += ball_dp_x * dt;
		ball_p_y += ball_dp_y * dt;
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, ballColor);


		if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 80, player_1_p, player_1_half_size_x, player_1_half_size_y)
			) {
			ball_p_x = 80 - player_half_size_x - ball_half_size;
			ball_dp_x *= -1;
			//ball_dp_y = player_1_dp *.75f; 
			ball_dp_y = (ball_p_y - player_1_p) * 2 + player_1_dp * .75f;
			ballColor = 0x00FFFF;
		}

		if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -80, player_2_p, player_2_half_size_x, player_2_half_size_y)
			) {
			ball_p_x = -80 + player_half_size_x + ball_half_size;
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player_2_p) * 2 + player_2_dp * .75f;
			ballColor = 0xFF6347;
		}



		if (ball_p_y + ball_half_size > arena_half_size_y) {
			ball_p_y = arena_half_size_y - ball_half_size;
			ball_dp_y *= -1;
			ballColor = 0xFFFF00;
		}
		else if (ball_p_y - ball_half_size < -arena_half_size_y) {
			ball_p_y = -arena_half_size_y + ball_half_size;
			ball_dp_y *= -1;
			ballColor = 0xFFFF00;
		}
		if (ball_p_x + ball_half_size > arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			player_1_score++;
			ballColor = 0xFF49B3;
			player_2_Answers++;
		}
		if (ball_p_x - ball_half_size < -arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			player_2_score++;
			ballColor = 0xFF49B3;
			player_1_Answers++;
		}

	}

	//HEALTH

	//float at_x = -80;
	//for (int i = 0; i < player_1_score; i++) {
	//	draw_rect(at_x, 47.f, 1.f, 1.f, 0xaaaaaa);
	//	at_x += 2.5f;
	//}
	//at_x = 80;
	//for (int i = 0; i < player_2_score; i++) {
	//	draw_rect(at_x, 47.f, 1.f, 1.f, 0xaaaaaa);
	//	at_x -= 2.5f;
	//}

	draw_number(player_1_score, -10, 40, 1.f, 0xbbffbb);
	draw_number(player_2_score, 10, 40, 1.f, 0xbbffbb);
	// Inside your gameplay function
	// Detect collision with player 3 on all sides
	if (player1.question == true && player2.question == true && player2.isCorrect == 2 && random2 <= 50) {
		static bool movingUp = true;
		const float movementSpeed = 20.0f; // Adjust the speed of movement
		const float maxUpPosition = 20.0f; // Adjust the maximum up position
		const float minDownPosition = -20.0f; // Adjust the minimum down position
		if (movingUp) {
			player_3_p += movementSpeed * dt; // Move up
			if (player_3_p >= maxUpPosition) {
				player_3_p = maxUpPosition; // Limit at the maximum up position
				movingUp = false; // Change direction
			}
		}
		else {
			player_3_p -= movementSpeed * dt; // Move down
			if (player_3_p <= minDownPosition) {
				player_3_p = minDownPosition; // Limit at the minimum down position
				movingUp = true; // Change direction
			}
		}
		simulate_player(&player_3_p, &player_3_dp, player_3_ddp, dt);
		if ((ball_p_x + ball_half_size > 30 - player_half_size_x) &&
			(ball_p_x - ball_half_size < 30 + player_half_size_x) &&
			(ball_p_y + ball_half_size > player_3_p - 17) &&
			(ball_p_y - ball_half_size < player_3_p + 17)) {
			// Handle collision with player 3 here
			if (ball_p_x < 30) {
				ball_p_x = 30 - player_half_size_x - ball_half_size;
			}
			else {
				ball_p_x = 30 + player_half_size_x + ball_half_size;
			}
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player_3_p) * 2 + player_3_dp * 0.75f;
			ballColor = 0x808080;
		}

		draw_rect(30, player_3_p, player_half_size_x, 17, 0xff0000);
		render_ascii_art(extraWallDebuff, 9, 7, -80, -35, 1, 0x888888, 0x888888, 0x888888);
		draw_text("EXTRA WALL", -65, -36.5, 0.5, 0x888888);
	}
	if (player1.question == true && player2.question == true && player1.isCorrect == 2 && random2 <= 50) {
		static bool movingUp2 = true;
		const float movementSpeed = 20.0f; // Adjust the speed of movement
		const float maxUpPosition = 20.0f; // Adjust the maximum up position
		const float minDownPosition = -20.0f; // Adjust the minimum down position
		if (movingUp2) {
			player_4_p += movementSpeed * dt; // Move up
			if (player_4_p >= maxUpPosition) {
				player_4_p = maxUpPosition; // Limit at the maximum up position
				movingUp2 = false; // Change direction
			}
		}
		else {
			player_4_p -= movementSpeed * dt; // Move down
			if (player_4_p <= minDownPosition) {
				player_4_p = minDownPosition; // Limit at the minimum down position
				movingUp2 = true; // Change direction
			}
		}
		simulate_player(&player_4_p, &player_4_dp, player_4_ddp, dt);
		if ((ball_p_x + ball_half_size > -30 - player_half_size_x) &&
			(ball_p_x - ball_half_size < -30 + player_half_size_x) &&
			(ball_p_y + ball_half_size > player_4_p - 17) &&
			(ball_p_y - ball_half_size < player_4_p + 17)) {
			// Handle collision with player 3 here
			if (ball_p_x < -30) {
				ball_p_x = -30 - player_half_size_x - ball_half_size;
			}
			else {
				ball_p_x = -30 + player_half_size_x + ball_half_size;
			}
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player_3_p) * 2 + player_4_dp * 0.75f;
			ballColor = 0x808080;
		}

		draw_rect(-30, player_4_p, player_half_size_x, 17, 0xff0000);
		render_ascii_art(extraWallDebuff, 9, 7, 70, -35, 1, 0x888888, 0x888888, 0x888888);
		draw_text("EXTRA WALL", 30, -36.5, 0.5, 0x888888);
	}
}

internal void testQuestionExtraGameplay(Input* input, int questionIndex, float dt) {
	pressShiftAnimation(dt);
	pressShiftQuestion(dt);
	pressQuicklyAnimation(dt);
	std::map<int, Question> questions = questionHighScore;

	// Seed the random number generator with the current time
	auto it = questions.find(questionIndex);

	if (it != questions.end()) {
		const Question& currentQuestion = it->second;

		// Extract question text, answer options, and correct answer from currentQuestion
		const char* questionText = currentQuestion.questionText;
		const std::vector<const char*>& answerOptions = currentQuestion.answerOptions;
		const char* correctAnswer = currentQuestion.correctAnswer;

		// Display the question
		draw_text(questionText, -80, 20, 0.6, 0xFFFF00);

		if (pressed(BUTTON_A)) {
			display = 1;
		}

		if (pressed(BUTTON_D)) {
			display = 2;
		}

		if (pressed(BUTTON_S)) {
			display = 3;
		}
		if (display == 1) {
			draw_text("A", -46, 8.8, 0.5, 0x48FFFF);
			draw_text(answerOptions[0], -58, 0, 0.45, 0x48FFFF);
			render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
			draw_text("D", 40.6, 8.8, 0.5, 0x888888);
			draw_text(answerOptions[1], 30, 0, 0.45, 0x888888);
			render_ascii_art(box, 29, 7, 38, 10, 1, 0x888888, 0x888888, 0x888888);
			draw_text("S", -3, -14, 0.5, 0x888888);
			draw_text(answerOptions[2], -12, -23.5, 0.45, 0x888888);
			render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
		}
		else if (display == 2) {
			draw_text("A", -46, 8.8, 0.5, 0x888888);
			draw_text(answerOptions[0], -58, 0, 0.45, 0x888888);
			render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x888888, 0x888888, 0x888888);
			draw_text("D", 40.6, 8.8, 0.5, 0x48FFFF);
			draw_text(answerOptions[1], 30, 0, 0.45, 0x48FFFF);
			render_ascii_art(box, 29, 7, 38, 10, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
			draw_text("S", -3, -14, 0.5, 0x888888);
			draw_text(answerOptions[2], -12, -23.5, 0.45, 0x888888);
			render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
		}
		else {
			draw_text("A", -46, 8.8, 0.5, 0x888888);
			draw_text(answerOptions[0], -58, 0, 0.45, 0x888888);
			render_ascii_art(box, 9, 7, -48.7, 10, 1, 0x888888, 0x888888, 0x888888);
			draw_text("D", 40.6, 8.8, 0.5, 0x888888);
			draw_text(answerOptions[1], 30, 0, 0.45, 0x888888);
			render_ascii_art(box, 9, 7, 38, 10, 1, 0x888888, 0x888888, 0x888888);
			draw_text("S", -3, -14, 0.5, 0x48FFFF);
			draw_text(answerOptions[2], -12, -23.5, 0.45, 0x48FFFF);
			render_ascii_art(box, 9, 7, -5.8, -12.8, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
		}

		if (pressed(BUTTON_SHIFT)) {
			// Check if the player selected the correct answer
			random2 = getRandomNumber(100);
			const char* playerChoice;
			if (display == 1) {
				playerChoice = answerOptions[0];
			}
			else if (display == 2) {
				playerChoice = answerOptions[1];
			}
			else {
				playerChoice = answerOptions[2];
			}

			if (strcmp(playerChoice, correctAnswer) == 0) {
				// Player answered correctly
				answer.isCorrect = 1;
				answer.question = true;
				random = getRandomNumber(15);
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;
				ball_p1_x = 0;
				ball_p1_y = 0;
				timer = 11;
			}
			else {

				// Player answered incorrectly
				answer.isCorrect = 2;
				answer.question = true;
				random = getRandomNumber(15);
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;
				ball_p1_x = 0;
				ball_p1_y = 0;
				timer = 11;
			}
		}
	}
}

int displayText = 0;

struct playerScore {
	std::string name;
	int score;

	// Default constructor
	playerScore() : name(""), score(0) {}

	// Constructor with parameters
	playerScore(std::string playerName, int playerScore) : name(playerName), score(playerScore) {}
};
const char* line[] = {
	"00000000",
};

int numberPlayers = 0;
std::map<int, playerScore> scoreMap; // Map to store playerScore with score as key
std::string text = "";
std::string text1 = "";
std::string text2 = "";
std::string text3 = "";

internal void pressKeyBoard(float dt) {
	// Define the colors for the flashing effect
	// Define the colors for the flashing effect
	u32 color1 = 0x48FFFF; // Bright yellow
	u32 color2 = 0x008B8B; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime5 += dt;

	// Check if enough time has passed to change colors
	if (useColor6) {
		if (elapsedTime5 >= flashingIntervalColor11) {
			useColor6 = false;
			elapsedTime5 = 0.0f;
		}
	}
	else {
		if (elapsedTime5 >= flashingIntervalColor12) {
			useColor6 = true;
			elapsedTime5 = 0.0f;
		}
	}


	if (useColor6) {
		draw_text("PRESS KEYBOARD TO CHOOSE LETTER", -34, 40, 0.4, color1);
	}
	else {
		draw_text("PRESS KEYBOARD TO CHOOSE LETTER", -34, 40, 0.4, color2);
	}
}

internal void individualLine(float x, float y, float dt) {

	// Define the colors for the flashing effect
	u32 color1 = 0x48FFFF; // Bright yellow
	u32 color2 = 0x008B8B; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime += dt;

	// Check if enough time has passed to change colors
	if (useColor1) {
		if (elapsedTime >= flashingIntervalColor1) {
			useColor1 = false;
			elapsedTime = 0.0f;
		}
	}
	else {
		if (elapsedTime >= flashingIntervalColor2) {
			useColor1 = true;
			elapsedTime = 0.0f;
		}
	}

	if (useColor1) {
		render_ascii_art(line, 1, 1, x, y, 1, color1, color1, color1);
	}
	else {
		render_ascii_art(line, 1, 1, x, y, 1, color2, color2, color2);
	}
}
float elapsedTime8 = 0.0;
bool found = false;
bool notSave = false;
internal void pickName(Input* input, float dt, int player_score) {
	playMusic3 = true;
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x292A50);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x23232B);
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_UI;
		player_1_score = 0;
		answer.isCorrect = 0;
		answer.question = false;
		timer = 11;
		displayText = 0;
		found = false;
		playMusic = true;
		playMusic2 = false;
		playMusic3 = false;
		elapsedTime8 = 0.0;
		ball_dp_x *= -1;
		ball_dp_y = 0;
		ball_p_x = 0;
		ball_p_y = 0;
		player_1_p = 0;
		player_2_p = 0;
		text = "";
		text1 = "";
		text2 = "";
	}
	if (pressed(BUTTON_ENTER)) {
		displayText = 3;
	}
	if (displayText < 3) {
		if (pressed(BUTTON_A)) {
			if (displayText == 0) {
				text = "A";
				displayText++;
			}
			else if (displayText == 1) {
				text1 = "A";
				displayText++;
			}
			else if (displayText == 2) {
				text2 = "A";
			}
		}
		else if (pressed(BUTTON_B)) {
			if (displayText == 0) {

				text = "B";
				displayText++;
			}
			else if (displayText == 1) {

				text1 = "B";
				displayText++;
			}
			else if (displayText == 2) {

				text2 = "B";

			}
		}
		else if (pressed(BUTTON_C)) {
			if (displayText == 0) {

				text = "C";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "C";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "C";

			}
		}
		else if (pressed(BUTTON_D)) {
			if (displayText == 0) {

				text = "D";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "D";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "D";
			}
		}
		else if (pressed(BUTTON_E)) {
			if (displayText == 0) {

				text = "E";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "E";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "E";
			}
		}
		else if (pressed(BUTTON_F)) {
			if (displayText == 0) {

				text = "F";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "F";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "F";
			}
		}
		else if (pressed(BUTTON_G)) {
			if (displayText == 0) {

				text = "G";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "G";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "G";
			}
		}
		else if (pressed(BUTTON_H)) {
			if (displayText == 0) {

				text = "H";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "H";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "H";
			}
		}
		else if (pressed(BUTTON_I)) {
			if (displayText == 0) {

				text = "I";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "I";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "I";
			}
		}
		else if (pressed(BUTTON_J)) {
			if (displayText == 0) {

				text = "J";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "J";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "J";
			}
		}
		else if (pressed(BUTTON_K)) {
			if (displayText == 0) {

				text = "K";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "K";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "K";
			}
		}
		else if (pressed(BUTTON_L)) {
			if (displayText == 0) {

				text = "L";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "L";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "L";
			}
		}
		else if (pressed(BUTTON_M)) {
			if (displayText == 0) {

				text = "M";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "M";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "M";
			}
		}
		else if (pressed(BUTTON_N)) {
			if (displayText == 0) {

				text = "N";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "N";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "N";
			}
		}
		else if (pressed(BUTTON_O)) {
			if (displayText == 0) {

				text = "O";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "O";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "O";
			}
		}
		else if (pressed(BUTTON_P)) {
			if (displayText == 0) {

				text = "P";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "P";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "P";
			}
		}
		else if (pressed(BUTTON_Q)) {
			if (displayText == 0) {

				text = "Q";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "Q";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "Q";
			}
		}
		else if (pressed(BUTTON_R)) {
			if (displayText == 0) {

				text = "R";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "R";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "R";
			}
		}
		else if (pressed(BUTTON_S)) {
			if (displayText == 0) {

				text = "S";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "S";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "S";
			}
		}
		else if (pressed(BUTTON_T)) {
			if (displayText == 0) {

				text = "T";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "T";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "T";
			}
		}
		else if (pressed(BUTTON_U)) {
			if (displayText == 0) {

				text = "U";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "U";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "U";
			}
		}
		else if (pressed(BUTTON_V)) {
			if (displayText == 0) {

				text = "V";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "V";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "V";
			}
		}
		else if (pressed(BUTTON_W)) {
			if (displayText == 0) {

				text = "W";
				displayText++;

			}
			else if (displayText == 1) {
				text1 = "W";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "W";
			}
		}
		else if (pressed(BUTTON_X)) {
			if (displayText == 0) {

				text = "X";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "X";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "X";
			}
		}
		else if (pressed(BUTTON_Y)) {
			if (displayText == 0) {

				text = "Y";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "Y";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "Y";
			}
		}
		else if (pressed(BUTTON_Z)) {
			if (displayText == 0) {

				text = "Z";
				displayText++;

			}
			else if (displayText == 1) {

				text1 = "Z";
				displayText++;

			}
			else if (displayText == 2) {

				text2 = "Z";
			}
		}
		else if (pressed(BUTTON_BACKSPACE)) {
			if (displayText == 0) {

				text = "";
			}
			else if (displayText == 1) {

				text1 = "";
				displayText--;

			}
			else if (displayText == 2) {

				text2 = "";
				displayText--;

			}
		}
	}
	if (displayText <= 2 && pressed(BUTTON_SPACE)) {
		if (displayText >= 2) {
			displayText = 0;
		}
		else {
			displayText++;
		}
	}
	if (displayText <= 2 && pressed(BUTTON_RIGHT)) {
		if (displayText >= 2) {
			displayText = 0;
		}
		else {
			displayText++;
		}
	}
	if (displayText >= 0 && pressed(BUTTON_LEFT)) {
		if (displayText <= 0) {
			displayText = 2;
		}
		else {
			displayText--;
		}

	}
	pressKeyBoard(dt);
	pressEnterAnimation(dt);
	pressEscapeAnimation(dt);
	draw_text(" TYPE YOUR NAME", -30, 20, 0.7, 0x8C68FF);
	draw_text(text.c_str(), -15, 0, 1, 0x06CFCE);
	draw_text(text1.c_str(), -1.5, 0, 1, 0x06CFCE);
	draw_text(text2.c_str(), 12, 0, 1, 0x06CFCE);
	if (displayText == 0) {
		individualLine(-17, -10, dt);
		render_ascii_art(line, 1, 1, -3.5, -10, 1, 0x06CFCE, 0x06CFCE, 0x06CFCE);
		render_ascii_art(line, 1, 1, 10, -10, 1, 0x06CFCE, 0x06CFCE, 0x06CFCE);
	}
	else if (displayText == 1) {
		render_ascii_art(line, 1, 1, -17, -10, 1, 0x06CFCE, 0x06CFCE, 0x06CFCE);
		individualLine(-3.5, -10, dt);
		render_ascii_art(line, 1, 1, 10, -10, 1, 0x06CFCE, 0x06CFCE, 0x06CFCE);
	}
	else if (displayText == 2) {
		render_ascii_art(line, 1, 1, -17, -10, 1, 0x06CFCE, 0x06CFCE, 0x06CFCE);
		render_ascii_art(line, 1, 1, -3.5, -10, 1, 0x06CFCE, 0x06CFCE, 0x06CFCE);
		individualLine(10, -10, dt);
	}
	else {
		render_ascii_art(line, 1, 1, -17, -10, 1, 0x06CFCE, 0x06CFCE, 0x06CFCE);
		render_ascii_art(line, 1, 1, -3.5, -10, 1, 0x06CFCE, 0x06CFCE, 0x06CFCE);
		render_ascii_art(line, 1, 1, 10, -10, 1, 0x06CFCE, 0x06CFCE, 0x06CFCE);
	}
	if (displayText >= 3) {
		std::string concatenatedText = text + text1 + text2;
		text3 = concatenatedText;
		if (text3 == "") {
			displayText = 0;
		}
		else {
			for (auto& entry : scoreMap) {
				if (entry.second.name == text3 && player_score > entry.second.score) {
					entry.second.score = player_score;
					found = true;
					break;
				}
				else if (entry.second.name == text3) {
					found = true;
					break;
				}
			}
			if (!found && numberPlayers <= 4) {
				scoreMap[numberPlayers] = playerScore(text3, player_score);
				numberPlayers++;
			}
			else if (!found) {
				notSave = true;
			}
			// Store in the map
			// Display or do something with the stored data
			// Display or use the stored data
			current_gamemode = GM_LEADERBOARDS;
		}
	}
}

internal void leaderboardsTitle(float dt) {
	// Define the colors for the flashing effect
	// Define the colors for the flashing effect
	u32 color1 = 0xFFFF00; // Bright yellow
	u32 color2 = 0xCC9900; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime5 += dt;

	// Check if enough time has passed to change colors
	if (useColor6) {
		if (elapsedTime5 >= flashingIntervalColor11) {
			useColor6 = false;
			elapsedTime5 = 0.0f;
		}
	}
	else {
		if (elapsedTime5 >= flashingIntervalColor12) {
			useColor6 = true;
			elapsedTime5 = 0.0f;
		}
	}


	if (useColor6) {
		draw_text("LEADERBOARDS", -22, 40, 0.7, color1);
	}
	else {
		draw_text("LEADERBOARDS", -22, 40, 0.7, color2);
	}
}

internal void leaderboards(Input* input, float dt) {

	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x292A50);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x23232B);
	std::vector<std::pair<int, playerScore>> sortedScores(scoreMap.begin(), scoreMap.end());

	std::sort(sortedScores.begin(), sortedScores.end(), [](const auto& a, const auto& b) {
		return a.second.score > b.second.score; // Sort in ascending order based on score
		});
	if (notSave == true) {
		draw_text("NAME IS NOT FOUND", -32.5, 40, 0.7, 0xFFFF00);
	}
	else {
		leaderboardsTitle(dt);
	}
	pressEscapeAnimation(dt);
	draw_text("NAME", -15, 30, 0.5, 0x8C68FF); // Manually set x and y positions for names
	draw_text("SCORE", 5, 30, 0.5, 0x8C68FF); // Manually set x and y positions for names
	int yOffset = 20;
	int yOffset1 = 18;
	for (const auto& pair : sortedScores) {
		const playerScore& score = pair.second;
		draw_text(score.name.c_str(), -15, yOffset, 0.7, 0x06CFCE); // Manually set x and y positions for names
		draw_number(score.score, 13, yOffset1, 1, 0x06CFCE); // Manually set x and y positions for scores
		yOffset -= 10;
		yOffset1 -= 10; // Increment y-offset for the next entry
	}
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_UI;
		player_1_score = 0;
		answer.isCorrect = 0;
		answer.question = false;
		timer = 11;
		displayText = 0;
		found = false;
		playMusic = true;
		playMusic2 = false;
		playMusic3 = false;
		elapsedTime8 = 0.0;
		ball_dp_x *= -1;
		ball_dp_y = 0;
		ball_p_x = 0;
		ball_p_y = 0;
		player_1_p = 0;
		player_2_p = 0;
		text = "";
		text1 = "";
		text2 = "";
	}
}

internal void deleteProgress(Input* input, float dt) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	if (pressed(BUTTON_A)) {
		up_down = 0;
	}
	if (pressed(BUTTON_D)) {
		up_down = 1;
	}
	draw_text("LEADERBOARDS IS FULL! DELETE PROGRESS?", -65, 30, 0.6, 0xFFFF00);
	if (up_down == 0) {
		draw_text("YES", -50, -10, 1, 0x48FFFF);
		draw_text("NO", 42, -10, 1, 0x888888);
		draw_text("GO TO LEADERBOARDS", -60, -25, 0.5, 0x48FFFF);
		if (pressed(BUTTON_ENTER)) {
			numberPlayers = 0;
			scoreMap.clear();
			current_gamemode = GM_NAME;
		}
	}
	else {
		draw_text("YES", -50, -10, 1, 0x888888);
		draw_text("NO", 42, -10, 1, 0x48FFFF);
		draw_text("UPDATE EXISTING SAVE", 0, -25, 0.5, 0x48FFFF);
		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_NAME;
		}
	}
}

bool respawn = false;
internal void highScore(Input* input, float dt) {
	float player_2_half_size_x = 2.5, player_2_half_size_y = 12;
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	timer -= dt;
	if (timer >= 1) {
		draw_text("TIME UNTIL QUESTION", -18, 42, 0.3, 0xbbffbb);
		draw_number(timer, 0, 35, 0.8, 0xbbffbb);
	}
	bool slowMo = false;
	if (timer <= 1) {
		slowMo = true;
		answer.question = false;
	}
	elapsedTime8 += dt;
	if (elapsedTime8 <= intervalAppear) {
		draw_text("PRESS [ESC] TO OPEN MENU", -80, -40, 0.5, 0xFFFF00);
		draw_text(">", -79.5, 30, 1, 0xFFFF00);
		draw_text("W", -79, 20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, -82.5, 22, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("S", -79, -20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, -82.5, -18, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("<", -79.5, -30, 1, 0xFFFF00);
	}
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_PAUSEHS;
	}
	float player_1_ddp = 0.f;
	player_1_ddp = (ball_p_y - player_1_p) * 100;
	if (player_1_ddp > 1300) player_1_ddp = 1300;
	if (player_1_ddp < -1300) player_1_ddp = -1300;
	float player_3_ddp = 0;
	float player_2_ddp = 0.f;
	if (answer.isCorrect == 1 && random2 <= 50) {
		if (is_down(BUTTON_W)) player_2_ddp += 5000;
		if (is_down(BUTTON_S)) player_2_ddp -= 5000;
		render_ascii_art(speedBoost, 9, 7, -80, -35, 1, 0x888888, 0x888888, 0x888888);
		draw_text("FASTER BOOST", -65, -36.5, 0.5, 0x888888);
	}
	else if (timer <= 1 && answer.question == false) {
		if (is_down(BUTTON_W)) player_2_ddp += 0;
		if (is_down(BUTTON_S)) player_2_ddp -= 0;

	}
	else {
		if (is_down(BUTTON_W)) player_2_ddp += 2000;
		if (is_down(BUTTON_S)) player_2_ddp -= 2000;
	}

	simulate_player(&player_1_p, &player_1_dp, player_1_ddp, dt);
	simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);
	if (timer <= 1 && answer.question == false) {
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0x000000);
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0x000000);
	}
	else if (answer.isCorrect == 1 && random2 >= 50) {
		player_2_half_size_y = 16;
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xff0000);
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xff0000);
		render_ascii_art(tallerBoost, 9, 7, -80, -35, 1, 0x888888, 0x888888, 0x888888);
		draw_text("TALLER BOOST", -65, -36.5, 0.5, 0x888888);
	}
	else if (answer.isCorrect == 2 && random2 >= 50) {
		player_2_half_size_y = 8;
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xff0000);
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xff0000);
		render_ascii_art(shorterDebuff, 9, 7, -80, -35, 1, 0x888888, 0x888888, 0x888888);
		draw_text("SHORTER DEBUFF", -65, -36.5, 0.5, 0x888888);
	}
	else {
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xff0000);
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xff0000);
	}
	if (timer <= 1) {
		testQuestionExtraGameplay(input, random, dt);
		draw_arena_borders_special(arena_half_size_x, arena_half_size_y, 0x383841, 0x383841, 0x383841);
	}
	if (!slowMo) {
		ball_p_x += ball_dp_x * dt;
		ball_p_y += ball_dp_y * dt;
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, ballColor);
	}
	else {
		ball_p1_x += -10 * dt;
		ball_p1_y = 0;
		draw_rect(ball_p1_x, ball_p1_y, ball_half_size, ball_half_size, 0xffffff);
		if (ball_p1_x - ball_half_size < -arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p1_x = 0;
			ball_p1_y = 0;
			timer = 11;
			slowMo = false;
			answer.question = true;
			answer.isCorrect = 2;
			random = getRandomNumber(15);
			random2 = getRandomNumber(100);
			player_high_score--;
		}
	}

	if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 80, player_1_p, player_half_size_x, player_half_size_y)
		) {
		ball_p_x = 80 - player_half_size_x - ball_half_size;
		ball_dp_x *= -1;
		//ball_dp_y = player_1_dp *.75f; 
		ball_dp_y = (ball_p_y - player_1_p) * 2 + player_1_dp * .75f;
		ballColor = 0x00FFFF;
	}

	if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -80, player_2_p, player_2_half_size_x, player_2_half_size_y)
		) {
		ball_p_x = -80 + player_half_size_x + ball_half_size;
		ball_dp_x *= -1;
		ball_dp_y = (ball_p_y - player_2_p) * 2 + player_2_dp * .75f;
		ballColor = 0xFF6347;
	}



	if (ball_p_y + ball_half_size > arena_half_size_y) {
		ball_p_y = arena_half_size_y - ball_half_size;
		ball_dp_y *= -1;
		ballColor = 0xFFFF00;
	}
	else if (ball_p_y - ball_half_size < -arena_half_size_y) {
		ball_p_y = -arena_half_size_y + ball_half_size;
		ball_dp_y *= -1;
		ballColor = 0xFFFF00;
	}
	if (ball_p_x + ball_half_size > arena_half_size_x) {
		ball_dp_x *= -1;
		ball_dp_y = 0;
		ball_p_x = 0;
		ball_p_y = 0;
		ballColor = 0xFF49B3;
		player_1_score++;

	}
	if (ball_p_x - ball_half_size < -arena_half_size_x) {
		ball_dp_x *= -1;
		ball_dp_y = 0;
		ball_p_x = 0;
		ball_p_y = 0;
		ballColor = 0xFF49B3;
		player_high_score--;
	};

	//HEALTH

	//float at_x = -80;
	//for (int i = 0; i < player_1_score; i++) {
	//	draw_rect(at_x, 47.f, 1.f, 1.f, 0xaaaaaa);
	//	at_x += 2.5f;
	//}
	//at_x = 80;
	//for (int i = 0; i < player_2_score; i++) {
	//	draw_rect(at_x, 47.f, 1.f, 1.f, 0xaaaaaa);
	//	at_x -= 2.5f;
	//}

	float at_x = -80;
	draw_text("HEALTH", -80, 40, 0.3, 0xbbffbb);
	for (int i = 0; i < player_high_score; i++) {
		draw_rect(at_x, 35, 1.f, 1.f, 0xbbffbb);
		at_x += 2.5f;
	}
	draw_text("SCORE", 74, 42, 0.3, 0xbbffbb);
	draw_number(player_1_score, 80, 35, 0.8, 0xbbffbb);
	// Inside your gameplay function
	// Detect collision with player 3 on all sides
	if (answer.isCorrect == 2 && !slowMo && random2 <= 50) {
		static bool movingUp = true;
		const float movementSpeed = 20.0f; // Adjust the speed of movement
		const float maxUpPosition = 20.0f; // Adjust the maximum up position
		const float minDownPosition = -20.0f; // Adjust the minimum down position
		if (movingUp) {
			player_3_p += movementSpeed * dt; // Move up
			if (player_3_p >= maxUpPosition) {
				player_3_p = maxUpPosition; // Limit at the maximum up position
				movingUp = false; // Change direction
			}
		}
		else {
			player_3_p -= movementSpeed * dt; // Move down
			if (player_3_p <= minDownPosition) {
				player_3_p = minDownPosition; // Limit at the minimum down position
				movingUp = true; // Change direction
			}
		}
		simulate_player(&player_3_p, &player_3_dp, player_3_ddp, dt);
		if ((ball_p_x + ball_half_size > 30 - player_half_size_x) &&
			(ball_p_x - ball_half_size < 30 + player_half_size_x) &&
			(ball_p_y + ball_half_size > player_3_p - 17) &&
			(ball_p_y - ball_half_size < player_3_p + 17)) {
			// Handle collision with player 3 here
			if (ball_p_x < 30) {
				ball_p_x = 30 - player_half_size_x - ball_half_size;
			}
			else {
				ball_p_x = 30 + player_half_size_x + ball_half_size;
			}
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player_3_p) * 2 + player_3_dp * 0.75f;
		}

		draw_rect(30, player_3_p, player_half_size_x, 17, 0x8D155E);
		render_ascii_art(extraWallDebuff, 9, 7, -80, -35, 1, 0x888888, 0x888888, 0x888888);
		draw_text("EXTRA WALL", -65, -36.5, 0.5, 0x888888);
	}
}


internal void pauseScreen(Input* input, float dt, int gamemode) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	if (pressed(BUTTON_W)) {
		up_down = 0;
	}
	if (pressed(BUTTON_S)) {
		up_down = 1;
	}

	if (up_down == 0) {
		draw_text("RESUME", -80, -10, 1, 0x48FFFF);
		draw_lowercase_letter("exit to menu", -80, -20, 0.7f, 0x888888);
		if (pressed(BUTTON_ENTER) && gamemode == 1) {
			current_gamemode = GM_GAMEPLAY;
		}
		else if (pressed(BUTTON_ENTER) && gamemode == 2) {
			current_gamemode = GM_MULTIPLAYER;
		}
		else if (pressed(BUTTON_ENTER) && gamemode == 3) {
			current_gamemode = GM_EXTRA_GAMEPLAY;
		}
	}
	else {
		draw_lowercase_letter("resume", -80, -10, 0.7f, 0x888888);
		draw_text("EXIT TO MENU", -80, -20, 1, 0x48FFFF);
		if (pressed(BUTTON_ENTER)) {
			playMusic = true;
			playMusic2 = false;
			playMusic5 = false;
			playMusic1 = false;
			playMusic6 = false;
			answer.isCorrect = 0;
			answer.question = false;
			timer = 11;
			player1.isCorrect = 0;
			player1.question = false;
			player2.isCorrect = 0;
			player2.question = false;
			answer1.isCorrect = 0;
			answer1.question = false;
			answer2.isCorrect = 0;
			answer2.question = false;
			elapsedTime8 = 0.0;
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			player_1_p = 0;
			player_2_p = 0;
			player_1_score = 0;
			player_2_score = 0;
			current_gamemode = GM_UI;
		}
	}
}
float yOffset;
float xOffset;
float titleX = 0.0f;
float titleY = 0.0f;

internal void quitScreen(Input* input, float dt) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	draw_text("QUIT?", -55, 35, 0.6, 0xFFFF00);
	if (pressed(BUTTON_A)) {
		up_down = 0;
	}
	if (pressed(BUTTON_D)) {
		up_down = 1;
	}
	if (up_down == 0) {
		draw_text("YES", -50, -10, 1, 0x48FFFF);
		draw_text("NO", 42, -10, 1, 0x888888);
		if (pressed(BUTTON_ENTER)) {
			quit = true;
		}
	}
	else {
		draw_text("YES", -50, -10, 1, 0x888888);
		draw_text("NO", 42, -10, 1, 0x48FFFF);
		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_UI;
		}
	}
}

internal void userUI(Input* input, float dt) {
	playMusic = true;
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	//render_ascii_art(extraWallDebuff, 9, 7, -5.8, -12.8, 1, 0x888888, 0x888888, 0x888888);
	xOffset = sin(titleX) * 2.0f; // Adjust the magnitude of movement
	yOffset = cos(titleY) * 2.0f; // Adjust the magnitude of movement
	titleX += 0.01f; // Adjust the speed of movement
	titleY += 0.007f; // Adjust the speed of movement
	WASD(dt);
	pressEnterAnimation(dt);
	//draw_text("PRESS [ENTER] TO CHOOSE", 10, -35, 0.5, 0xFFFF00);
	draw_text("PONG THE SEQUEL", -38 + xOffset, 30 + yOffset, 1, 0xFFFF00);
	if (pressed(BUTTON_A) || pressed(BUTTON_D)) {
		hot_button = !hot_button;
	}
	if (pressed(BUTTON_W)) {
		up_down--;
	}
	if (pressed(BUTTON_S)) {
		up_down++;
	}
	if (up_down < 0) {
		up_down = 2;
	}
	if (up_down > 2) {
		up_down = 0;
	}

	if (up_down == 0) {
		if (hot_button == 0) {
			draw_text("SINGLE PLAYER", -80, 0, 1, 0x48FFFF);
			draw_lowercase_letter("multiplayer", 30, 0, 0.7f, 0x888888);
			draw_lowercase_letter("high score", -80, -10, 0.7f, 0x888888);
			draw_lowercase_letter("tutorial", 30, -10, 0.7f, 0x888888);
			draw_lowercase_letter("quit", -10, -20, 1, 0x888888);
			if (pressed(BUTTON_ENTER) && gameplayTime != 0) {
				random = getRandomNumber(4);
				current_gamemode = GM_JEOPARDY;
				enemy_is_ai = hot_button ? 0 : 1;
				player_Answers = 0;
			}
			else if (pressed(BUTTON_ENTER) && gameplayTime == 0) {
				current_gamemode = GM_TUTORIAL_SINGLE_MENU;
			}
		}
		else {
			draw_lowercase_letter("single player", -80, 0, 0.7f, 0x888888);
			draw_text("MULTIPLAYER", 20, 0, 1, 0x48FFFF);
			draw_lowercase_letter("high score", -80, -10, 0.7f, 0x888888);
			draw_lowercase_letter("tutorial", 30, -10, 0.7f, 0x888888);
			draw_lowercase_letter("quit", -10, -20, 1, 0x888888);
			if (pressed(BUTTON_ENTER) && gameplayTime != 0) {
				random3 = getRandomNumber(4);
				player_2_Answers = 0;
				player_1_Answers = 0;
				current_gamemode = GM_JEOPARDY_MP;
			}
			else if (pressed(BUTTON_ENTER) && gameplayTime == 0) {
				current_gamemode = GM_TUTORIAL_MULTIPLAYER_MENU;
			}
		}
	}
	else if (up_down == 1) {
		if (hot_button == 0) {
			draw_lowercase_letter("single player", -80, 0, 0.7f, 0x888888);
			draw_lowercase_letter("multiplayer", 30, 0, 0.7f, 0x888888);
			draw_text("HIGH SCORE", -80, -10, 1, 0x48FFFF);
			draw_lowercase_letter("tutorial", 30, -10, 0.7f, 0x888888);
			draw_lowercase_letter("quit", -10, -20, 1, 0x888888);
			if (pressed(BUTTON_ENTER)) {
				playMusic = false;
				playMusic6 = true;
				random = getRandomNumber(15);
				player_high_score = 5;
				timer = 11;
				current_gamemode = GM_EXTRA_GAMEPLAY;
			}
		}
		else {
			draw_lowercase_letter("single player", -80, 0, 0.7f, 0x888888);
			draw_lowercase_letter("multiplayer", 30, 0, 0.7f, 0x888888);
			draw_lowercase_letter("high score", -80, -10, 0.7f, 0x888888);
			draw_text("TUTORIAL", 30, -10, 1, 0x48FFFF);
			draw_lowercase_letter("quit", -10, -20, 1, 0x888888);
			if (pressed(BUTTON_ENTER)) {
				gameplayTime++;
				player_Tutorial_Score = 0;
				current_gamemode = GM_TUTORIAL_SINGLE;
			}
		}
	}
	else if (up_down == 2) {
		draw_lowercase_letter("single player", -80, 0, 0.7f, 0x888888);
		draw_lowercase_letter("multiplayer", 30, 0, 0.7f, 0x888888);
		draw_lowercase_letter("high score", -80, -10, 0.7f, 0x888888);
		draw_lowercase_letter("tutorial", 30, -10, 0.7f, 0x888888);
		draw_text("QUIT", -10, -20, 1, 0x48FFFF);
		if (pressed(BUTTON_ENTER)) {
			titleX = 0.0; // Adjust the speed of movement
			titleY = 0.0; // Adjust the speed of movement
			current_gamemode = GM_QUIT;
		}
	}
}

internal void winPlayer(Input* input, float dt) {
	clear_screen(0xffaa33);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x292A50);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x23232B);
	pressEscapeAnimation(dt);
	if (player_1_score > player_2_score) {
		draw_text("PLAYER ONE MURKED PLAYER TWO!", -68, 30, 0.8, 0x8C68FF);
	}
	else {
		draw_text("PLAYER TWO MURKED PLAYER ONE!", -68, 30, 0.8, 0x8C68FF);
	}
	if (player_2_score > player_1_score) {
		draw_text("P", -40, 0, 1.5, 0x06CFCE);
		draw_number(1, -32.5, -4.6, 2.2, 0x06CFCE);
	}
	else {
		draw_text("P", -40, 0, 1.5, 0xFF0C0C);
		draw_number(1, -32.5, -4.6, 2.2, 0xFF0C0C);
	}
	if (player_1_score > player_2_score) {
		draw_text("P", 30, 0, 1.5, 0x06CFCE);
		draw_number(2, 40, -4.5, 2.15, 0x06CFCE);
	}
	else {
		draw_text("P", 30, 0, 1.5, 0xFF0C0C);
		draw_number(2, 40, -4.5, 2.15, 0xFF0C0C);
	}
	draw_number(player_1_score, -45, -20, 1.5, 0xbbffbb);
	draw_lowercase_letter("pts", -35, -18, 0.5, 0xbbffbb);
	draw_number(player_2_score, 27.5, -20, 1.5, 0xbbffbb);
	draw_lowercase_letter("pts", 37.5, -18, 0.5, 0xbbffbb);
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_UI;
		player_1_score = 0;
		player_2_score = 0;
		player1.isCorrect = 0;
		player_1_Answers = 0;
		player_2_Answers = 0;
		player1.question = false;
		player2.isCorrect = 0;
		player2.question = false;
		playMusic3 = false;
	}
}

internal void winScreen(Input* input, float dt) {

	clear_screen(0xffaa33);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x292A50);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x23232B);
	pressEscapeAnimation(dt);
	draw_text("YOU MURKED THE AI!", -50, 30, 1, 0x8C68FF);
	draw_text("P", -18, 20, 1, 0x06CFCE);
	draw_number(1, -13, 17, 1.4, 0x06CFCE);
	draw_lowercase_letter("wins", 0, 20, 1, 0xECECED);
	draw_text("P", -40, 0, 1.5, 0x06CFCE);
	draw_number(1, -32.5, -4.6, 2.2, 0x06CFCE);
	draw_number(player_1_score, -45, -20, 1.5, 0xbbffbb);
	draw_lowercase_letter("pts", -35, -18, 0.5, 0xbbffbb);
	draw_text("AI", 30, 0, 1.5, 0xFF0C0C);
	draw_number(player_2_score, 27.5, -20, 1.5, 0xbbffbb);
	draw_lowercase_letter("pts", 37.5, -18, 0.5, 0xbbffbb);
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_UI;
		player_1_score = 0;
		player_2_score = 0;
		answer1.isCorrect = 0;
		answer1.question = false;
		answer2.isCorrect = 0;
		player_Answers = 0;
		answer2.question = false;
		bluePowerup = false;
		redPowerup = false;
		question = false;
		playMusic3 = false;
		playMusic2 = false;
		loadingFinished.store(false); // Reset the atomic flag for future loading
	}
}

internal void loseScreen(Input* input, float dt) {
	clear_screen(0xffaa33);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x292A50);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x23232B);
	pressEscapeAnimation(dt);
	draw_text("YOU GOT YOUR BACK BLOWN OUT!", -60, 30, 0.75, 0x8C68FF);
	draw_text("AI", -18, 20, 1, 0xFF0C0C);
	draw_lowercase_letter("wins", 0, 20, 1, 0xECECED);
	draw_text("P", -40, 0, 1.5, 0x06CFCE);
	draw_number(1, -32.5, -4.6, 2.2, 0x06CFCE);
	draw_number(player_1_score, -45, -20, 1.5, 0xbbffbb);
	draw_lowercase_letter("pts", -35, -18, 0.5, 0xbbffbb);
	draw_text("AI", 30, 0, 1.5, 0xFF0C0C);
	draw_number(player_2_score, 27.5, -20, 1.5, 0xbbffbb);
	draw_lowercase_letter("pts", 37.5, -18, 0.5, 0xbbffbb);
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_UI;
		player_1_score = 0;
		player_2_score = 0;
		answer1.isCorrect = 0;
		answer1.question = false;
		player_Answers = 0;
		answer2.isCorrect = 0;
		answer2.question = false;
		bluePowerup = false;
		redPowerup = false;
		question = false;
		playMusic4 = false;
		playMusic2 = false;
		loadingFinished.store(false); // Reset the atomic flag for future loading
	}
}

internal void simulate_game(Input* input, float dt) {


	if (current_gamemode == GM_GAMEPLAY) {
		gameplay(input, dt);
		if (player_1_score >= 10) {
			playMusic1 = false;
			playMusic5 = true;
		}
		else {
			playMusic1 = true;
			playMusic = false;
		}
		if (player_1_score >= 20) {
			current_gamemode = GM_WINSCREEN;
		}
		else if (player_2_score >= 20) {
			current_gamemode = GM_LOSESCREEN;
		}
	}
	else if (current_gamemode == GM_DELETE) {
		deleteProgress(input, dt);
	}
	else if (current_gamemode == GM_EXTRA_GAMEPLAY) {
		highScore(input, dt);
		if (player_high_score <= 0) {
			current_gamemode = GM_NAME;
			if (numberPlayers > 4) {
				current_gamemode = GM_DELETE;
			}
		}
	}
	else if (current_gamemode == GM_MULTIPLAYER) {
		playMusic = false;
		playMusic2 = true;
		multiplayer(input, dt);
		if (player_1_score >= 20 || player_2_score >= 20) {
			current_gamemode = GM_WINPLAYER;
		}
	}
	else if (current_gamemode == GM_TUTORIAL_SINGLE_MENU) {
		tutorialSingleMenu(input, dt, 1);
	}
	else if (current_gamemode == GM_TUTORIAL_MULTIPLAYER_MENU) {
		tutorialSingleMenu(input, dt, 2);
	}
	else if (current_gamemode == GM_TUTORIAL_SINGLE) {
		playMusic = false;
		playMusic1 = true;
		tutorialSingle(input, dt);
	}
	else if (current_gamemode == GM_JEOPARDY_MP) {
		jeopardy(input, dt, 2);

	}
	else if (current_gamemode == GM_WINPLAYER) {
		playMusic2 = false;
		playMusic3 = true;
		winPlayer(input, dt);
	}
	else if (current_gamemode == GM_WINSCREEN) {
		playMusic5 = false;
		playMusic3 = true;
		clear_screen(0xffaa33);
		winScreen(input, dt);
	}
	else if (current_gamemode == GM_LOSESCREEN) {
		playMusic5 = false;
		playMusic4 = true;
		clear_screen(0xffaa33);
		loseScreen(input, dt);
	}
	else if (current_gamemode == GM_JEOPARDY) {
		jeopardy(input, dt, 1);
	}
	else if (current_gamemode == GM_PAUSESP) {
		pauseScreen(input, dt, 1);
	}
	else if (current_gamemode == GM_PAUSEMP) {
		pauseScreen(input, dt, 2);
	}
	else if (current_gamemode == GM_PAUSEHS) {
		pauseScreen(input, dt, 3);
	}
	else if (current_gamemode == GM_NAME) {
		playMusic6 = false;
		playMusic3 = true;
		pickName(input, dt, player_1_score);
	}
	else if (current_gamemode == GM_LEADERBOARDS) {
		leaderboards(input, dt);
	}
	else if (current_gamemode == GM_QUIT) {
		playMusic = false;
		playMusic2 = false;
		playMusic3 = false;
		playMusic4 = false;
		playMusic5 = false;
		playMusic1 = false;
		playMusic6 = false;
		quitScreen(input, dt);
	}
	else {
		userUI(input, dt);
	}
}