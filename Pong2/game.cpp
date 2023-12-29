#include <string>
#include<cstdlib>
#include <map>
#include <vector>
#include <windows.h>
#include <thread>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <ctime>

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
static bool playMusic = false;
static bool playMusic1 = false;
static bool playMusic2 = false;
static bool playMusic3 = false;
static bool playMusic4 = false;
static bool playMusic5 = false;
static bool quit = false;

//float player_pos_x = 0.f;
float player_1_p, player_1_dp, player_2_p, player_2_dp, player_3_p = 0, player_3_dp, player_3_ddp = 0;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;
float ball_p_x, ball_p_y, ball_dp_x = 130, ball_dp_y, ball_half_size = 2;
float ball_p1_x, ball_p1_y, ball_dp1_x = 130, ball_dp1_y, ball_half1_size = 2;

int player_1_score, player_2_score;

struct Question {
	const char* questionText;
	std::vector<const char*> answerOptions;
	const char* correctAnswer;
};

//Handles constraints for the player in the game
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

//Handles collision detection
internal bool
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {
	return (p1x + hs1x > p2x - hs2x &&
		p1x - hs1x < p2x + hs2x &&
		p1y + hs1y > p2y - hs2y &&
		p1y + hs1y < p2y + hs2y);
}


float flashingIntervalColor5 = 0.3f; // Change to color2 every 0.3 seconds
float flashingIntervalColor6 = 0.8f; // Change to color1 every 0.8 seco
float elapsedTime2 = 0.0f; // Track time elapsed
bool useColor3 = true; // Flag to toggle between colors

internal void pressQuicklyAnimation(float dt) {
	// Define the colors for the flashing effect
	u32 color1 = 0xFFFF00; // Bright yellow
	u32 color2 = 0xCC9900; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime2 += dt;

	// Check if enough time has passed to change colors
	if (useColor3) {
		if (elapsedTime2 >= flashingIntervalColor5) {
			useColor3 = false;
			elapsedTime2 = 0.0f;
		}
	}
	else {
		if (elapsedTime2 >= flashingIntervalColor6) {
			useColor3 = true;
			elapsedTime2 = 0.0f;
		}
	}

	if (useColor3) {
		draw_text("CHOOSE THE ANSWERS QUICKLY BEFORE THE BALL HITS YOUR GOAL", -80, 30, 0.3, color1);
	}
	else {
		draw_text("CHOOSE THE ANSWERS QUICKLY BEFORE THE BALL HITS YOUR GOAL", -80, 30, 0.3, color2);
	}
}

enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY,
	GM_EXTRA_GAMEPLAY,
	GM_UI,
	GM_WINSCREEN,
	GM_LOSESCREEN,
	GM_JEOPARDY,
	GM_WINPLAYER,
	GM_MULTIPLAYER,
	GM_LOADING,
	GM_PAUSE,
};

//global variables for use in methods
Gamemode current_gamemode;
int hot_button;
int up_down;
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

//random number generator
int getRandomNumber(int range) {
	// Seed the random number generator using current time
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// Generate a random number between 0 and 4
	return std::rand() % range; // Modulus 4 ensures numbers between 0 and 3 (inclusive)
}

//struct for answer used in test question
struct Answer {
	bool question;   // Text representing the answer option
	bool isCorrect;     // Indicates whether the answer is correct or not

	// Constructor to initialize the Answer struct
	Answer(bool question, bool isCorrect) : question(question), isCorrect(isCorrect) {}
};

Answer answer1(false, false);

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


// animateUI function
internal void pressEscapeAnimation(float dt) {
	float flashingIntervalColor3 = 0.3f; // Change to color2 every 0.3 seconds
	float flashingIntervalColor4 = 0.8f; // Change to color1 every 0.8 seco
	float elapsedTime1 = 0.0f; // Track time elapsed
	bool useColor2 = true; // Flag to toggle between colors
	// Define the colors for the flashing effect
	u32 color1 = 0xAB47BC; // Bright yellow
	u32 color2 = 0x6A1B9A; // Darker yellow (adjust as needed)

	// Inside your update/render loop or where time is tracked
	// Increment elapsed time by delta time (dt)
	elapsedTime1 += dt;

	// Check if enough time has passed to change colors
	if (useColor1) {
		if (elapsedTime1 >= flashingIntervalColor3) {
			useColor2 = false;
			elapsedTime1 = 0.0f;
		}
	}
	else {
		if (elapsedTime1 >= flashingIntervalColor4) {
			useColor2 = true;
			elapsedTime1 = 0.0f;
		}
	}

	if (useColor1) {
		draw_text("PRESS [ESC] TO RETURN TO MENU", -80, -35, 0.4, color1);
	}
	else {
		draw_text("PRESS [ESC] TO RETURN TO MENU", -80, -35, 0.4, color2);
	}
}


static std::map<int, Question> questionAlgo = {
		{0, {"WHAT IS THE AVERAGE CASE FOR STACK ?", {"ONE", "N", "N SQUARED"}, "ONE"}},
		{1, {"WHAT IS ONE OF THE COMPLEXITY ?", {"TIME", "DIMENSIONS", "GRAVITY"}, "TIME"}},
		{2, {"WHAT IS THE COMPLEXITY FOR THE KNAPSACK ?", {"N SQUARED", "N", "ONE"}, "N SQUARED"}},
		{3, {"WHAT IS THE COMPLEXITY FOR DISTRIBUTION ?", {"N", "ONE", "N SQUARED"}, "N"}}
};
static std::map<int, Question> questionAlgo1 = {
		{0, {"WHAT IS THE AVERAGE CASE FOR STACK ?", {"ONE", "N", "N SQUARED"}, "ONE"}},
		{1, {"WHAT IS ONE OF THE COMPLEXITY ?", {"TIME", "DIMENSIONS", "GRAVITY"}, "TIME"}}
};
static std::map<int, Question> questionAlgo2 = {
		{0, {"WHAT IS THE COMPLEXITY FOR THE KNAPSACK ?", {"N SQUARED", "N", "ONE"}, "N SQUARED"}},
		{1, {"WHAT IS THE COMPLEXITY FOR DISTRIBUTION ?", {"N", "ONE", "N SQUARED"}, "N"}}
};
static std::map<int, Question> questionJava = {
		{0, {"WHAT IS THE ENTRY POINT METHOD IN JAVA ?", {"MAIN", "START", "RUN"}, "MAIN"}},
		{1, {"WHAT KEYWORD IS USED TO DEFINE CONSTANT?", {"VAR", "FINAL", "CONST"}, "FINAL"}},
		{2, {"WHAT IS A SUPERCLASS?", {"MAIN", "SUPER", "OBJECT"}, "OBJECT"}},
		{3, {"WHICH KEYWORD IS USED FOR INHERITANCE?", {"EXTENDS", "INHERIT", "GIVEN"}, "EXTENDS"}}
};
static std::map<int, Question> questionBITS = {
		{0, {"TEAMWORK ISSUES THAT CAN HAPPEN", {"UNCLEAR GOALS", "FIST FIGHTING", "STABBING"}, "UNCLEAR GOALS"}},
		{1, {"IS BUDGET A REQUIRED PART OF THE PROPOSAL ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{2, {"WHY DO YOU NEED TO TEST SOFTWARE?", {"FUN", "ENGAGING", "SECURITY"}, "SECURITY"}},
		{3, {"WHAT IS CONDISERED GOOD PRACTICE OF CODING", {"WORD OF MOUTH", "COMMENTS", "MESSENGER"}, "COMMENTS"}}
};
static std::map<int, Question> questionML = {
		{0, {"WHAT IS THE GOAL OF SUPERVISED LEARNING?", {"PREDICTIONS", "PATTERNS", "SUMMARIZE"}, "PREDICTIONS"}},
		{1, {"WHAT IS THE GOAL OF UNSUPERVISED LEARNING?", {"PREDICTIONS", "SUMMARIZE", "PATTERNS"}, "PATTERNS"}},
		{2, {"WHAT TO DO TO EVALUATE CLASSIFICATION?", {"CONFUSION MATRIX", "FEATURE EXTRACTION", "TUNING"}, "CONFUSION MATRIX"}},
		{3, {"WHAT IS ENSEMBLE LEARNING?", {"SINGULAR APPROACH", "MULTIPLE MODELS", "ISOLATION PREDICTIONS"}, "MULTIPLE MODELS"}}
};
int pointerX;
int pointerY;
internal void jeopardy(Input* input, float dt) {
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
			if (pressed(BUTTON_ENTER)) {
				playMusic1 = true;
				playMusic = false;
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 1;
			}
		}
		else {
			draw_lowercase_letter("algorithm", -80, 20, 0.7f, 0x888888);
			draw_text("JAVA", 20, 20, 1, 0x48FFFF);
			draw_lowercase_letter("bits", -80, 10, 0.7f, 0x888888);
			draw_lowercase_letter("ml", 20, 10, 0.7f, 0x888888);
			draw_text("QUESTION RELATING TO THE JAVA LANGUAGE.", -80, -10, 0.4, 0xFFFFFF);
			draw_text("ASKS ABOUT THE KNOWLEDGE OF JAVA.", -80, -20, 0.4, 0xFFFFFF);
			if (pressed(BUTTON_ENTER)) {
				playMusic1 = true;
				playMusic = false;
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 2;
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
			if (pressed(BUTTON_ENTER)) {
				playMusic1 = true;
				playMusic = false;
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 3;
			}
		}
		else {
			draw_lowercase_letter("algorithm", -80, 20, 0.7f, 0x888888);
			draw_lowercase_letter("java", 20, 20, 0.7f, 0x888888);
			draw_lowercase_letter("bits", -80, 10, 0.7f, 0x888888);
			draw_text("ML", 20, 10, 1, 0x48FFFF);
			draw_text("QUESTION RELATING TO MACHINE LEARNING.", -80, -10, 0.4, 0xFFFFFF);
			draw_text("ASKS ABOUT THE MACHINE LEARNING CONCEPTS.", -80, -20, 0.4, 0xFFFFFF);
			if (pressed(BUTTON_ENTER)) {
				playMusic1 = true;
				playMusic = false;
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 4;
			}
		}
	}
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_UI;
	}
}

internal void pauseScreen(Input* input, float dt) {
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
		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_GAMEPLAY;
		}
	}
	else {
		draw_lowercase_letter("resume", -80, -10, 0.7f, 0x888888);
		draw_text("EXIT TO MENU", -80, -20, 1, 0x48FFFF);
		if (pressed(BUTTON_ENTER)) {
			playMusic = true;
			playMusic2 = false;
			playMusic5 = false;
			player_1_score = 0;
			player_2_score = 0;
			current_gamemode = GM_UI;
		}
	}
}

const char* box[] = {
	"00000000",
	"0      0",
	"0      0",
	"0      0",
	"0      0",
	"0      0",
	"00000000",
};

internal void testQuestion(Input* input, int questionIndex, float dt) {
	pressEnterAnimation(dt);
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
		draw_text(questionText, -80, 20, 0.4, 0xFFFF00);


		if (pressed(BUTTON_I)) {
			display = 1;
		}

		if (pressed(BUTTON_O)) {
			display = 2;
		}

		if (pressed(BUTTON_K)) {
			display = 3;
		}
		if (display == 1) {

			draw_text("I", -78, 0.5, 0.5, 0x48FFFF);
			draw_text(answerOptions[0], -80, -10, 0.6, 0x48FFFF);
			render_ascii_art(box, 9, 7, -81, 2, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
			draw_text("O", 20.7, 0.5, 0.5, 0x888888);
			draw_text(answerOptions[1], 20, -10, 0.6, 0x888888);
			render_ascii_art(box, 9, 7, 18, 2, 1, 0x888888, 0x888888, 0x888888);
			draw_text("K", -78.4, -20.5, 0.5, 0x888888);
			draw_text(answerOptions[2], -80, -30, 0.6, 0x888888);
			render_ascii_art(box, 9, 7, -81, -19, 1, 0x888888, 0x888888, 0x888888);
		}
		else if (display == 2) {
			draw_text("I", -78, 0.5, 0.5, 0x888888);
			draw_text(answerOptions[0], -80, -10, 0.6, 0x888888);
			render_ascii_art(box, 9, 7, -81, 2, 1, 0x888888, 0x888888, 0x888888);
			draw_text("O", 20.7, 0.5, 0.5, 0x48FFFF);
			draw_text(answerOptions[1], 20, -10, 0.6, 0x48FFFF);
			render_ascii_art(box, 9, 7, 18, 2, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
			draw_text("K", -78.4, -20.5, 0.5, 0x888888);
			draw_text(answerOptions[2], -80, -30, 0.6, 0x888888);
			render_ascii_art(box, 9, 7, -81, -19, 1, 0x888888, 0x888888, 0x888888);
		}
		else{
			draw_text("I", -78, 0.5, 0.5, 0x888888);
			draw_text(answerOptions[0], -80, -10, 0.6, 0x888888);
			render_ascii_art(box, 9, 7, -81, 2, 1, 0x888888, 0x888888, 0x888888);
			draw_text("O", 20.7, 0.5, 0.5, 0x888888);
			draw_text(answerOptions[1], 20, -10, 0.6, 0x888888);
			render_ascii_art(box, 9, 7, 18, 2, 1, 0x888888, 0x888888, 0x888888);
			draw_text("K", -78.4, -20.6, 0.5, 0x48FFFF);
			draw_text(answerOptions[2], -80, -30, 0.6, 0x48FFFF);
			render_ascii_art(box, 9, 7, -81, -19, 1, 0x48FFFF, 0x48FFFF, 0x48FFFF);
		}

		if (pressed(BUTTON_ENTER)) {
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
				// Player answered correctly
				answer1.isCorrect = true;
				answer1.question = true;
				//question = true;f
			}
			else {

				// Player answered incorrectly
				answer1.isCorrect = false;
				answer1.question = true;
			}
		}
	}
}

u32 ballColor = 0xFF49B3;
u32 slideColor = 0x6666FF * 2;
float elapsedTime3 = 0.0;
float intervalAppear = 3.0;
u32 lower;
u32 upper;
float tracking_speed_factor;
float overall_speed;
internal void gameplay(Input* input, float dt) {
	
	float player_2_half_size_x = 2.5, player_2_half_size_y = 12;
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders_special(arena_half_size_x, arena_half_size_y, 0x464651, lower, upper);
	elapsedTime3 += dt;
	if (elapsedTime3 <= intervalAppear) {
		draw_text("PRESS [ESC] TO OPEN MENU", -80, 35, 0.5, 0xFFFF00);
		draw_text(">", -79.5, 30, 1, 0xFFFF00);
		draw_text("W", -79, 20, 1, 0xFFFF00);
		render_ascii_art(box, 9,7,-82.5, 22, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("S", -79, -20, 1, 0xFFFF00);
		render_ascii_art(box, 9, 7, -82.5, -18, 1.6, 0xFFFF00, 0xFFFF00, 0xFFFF00);
		draw_text("<", -79.5, -30, 1, 0xFFFF00);

	}
	if (pressed(BUTTON_ESCAPE)) {
		current_gamemode = GM_PAUSE;
	}
	bool slowMo = false;
	float player_1_ddp = 0.f;
	if (player_1_score >= 6) {
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
	if (answer1.isCorrect == true && random2 == 0) {
		if (is_down(BUTTON_W)) player_2_ddp += 5000;
		if (is_down(BUTTON_S)) player_2_ddp -= 5000;
		slideColor = 0xb10000;
	}
	else if (slowMo) {
		player_2_p = 0;
		if (is_down(BUTTON_W)) player_2_ddp += 0;
		if (is_down(BUTTON_S)) player_2_ddp -= 0;
	}
	else {
		if (is_down(BUTTON_W)) player_2_ddp += 2000;
		if (is_down(BUTTON_S)) player_2_ddp -= 2000;
	}

	simulate_player(&player_1_p, &player_1_dp, player_1_ddp, dt);
	simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);
	if (answer1.isCorrect == true && random2 == 1) {
		player_2_half_size_y = 16;
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xD6178F);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0x964B00);
	}
	else if (answer1.isCorrect == false && answer1.question == true && random2 == 1) {
		player_2_half_size_y = 10;
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xD6178F);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xADD8E6);
	}
	else if (player_1_score == 5 && answer1.question == false) {
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0x000000);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0x000000);
	}
	else {
		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xD6178F);
		draw_rect(-80, player_2_p, player_2_half_size_x, player_2_half_size_y, 0xCCCDFF);
	}
	if (player_1_score == 5 && answer1.question == false) {
		countLong = 1;
		slowMo = true;
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
	if (answer1.question == true && answer1.isCorrect == false && random2 == 0) {
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
	}
}


internal void extraGameplay(Input* input, float dt) {
}
float yOffset;
float xOffset;
float titleX = 0.0f;
float titleY = 0.0f;

internal void userUI(Input* input, float dt) {

	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x464651);
	xOffset = sin(titleX) * 2.0f; // Adjust the magnitude of movement
	yOffset = cos(titleY) * 2.0f; // Adjust the magnitude of movement
	titleX += 0.01f; // Adjust the speed of movement
	titleY += 0.007f; // Adjust the speed of movement

	pressEnterAnimation(dt);

	//draw_text("PRESS [ENTER] TO CHOOSE", 10, -35, 0.5, 0xFFFF00);
	draw_text("PONG THE SEQUEL", -38 + xOffset, 30 + yOffset, 1, 0xFFFF00);
	if (pressed(BUTTON_A) || pressed(BUTTON_D)) {
		hot_button = !hot_button;
	}
	if (pressed(BUTTON_W)) {
		up_down = 0;
	}
	if (pressed(BUTTON_S)) {
		up_down = 1;
	}

	if (up_down == 0) {
		if (hot_button == 0) {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0x48FFFF);
			draw_lowercase_letter("multiplayer", 20, -10, 0.7f, 0x888888);
			draw_lowercase_letter("extra mode", -80, -20, 0.7f, 0x888888);
			draw_lowercase_letter("quit", 20, -20, 0.7f, 0x888888);
			if (pressed(BUTTON_ENTER)) {
				random2 = getRandomNumber(2);
				current_gamemode = GM_JEOPARDY;
				enemy_is_ai = hot_button ? 0 : 1;
			}
		}
		else {
			draw_lowercase_letter("single player", -80, -10, 0.7f, 0x888888);
			draw_text("MULTIPLAYER", 20, -10, 1, 0x48FFFF);
			draw_lowercase_letter("extra mode", -80, -20, 0.7f, 0x888888);
			draw_lowercase_letter("quit", 20, -20, 0.7f, 0x888888);
			if (pressed(BUTTON_ENTER)) {
				playMusic = false;
				playMusic2 = true;
				random3 = rand() % 2;
				current_gamemode = GM_MULTIPLAYER;
			}
		}
	}
	else if (up_down == 1) {
		if (hot_button == 0) {
			draw_lowercase_letter("single player", -80, -10, 0.7f, 0x888888);
			draw_lowercase_letter("multiplayer", 20, -10, 0.7f, 0x888888);
			draw_text("EXTRA MODE", -80, -20, 1, 0x48FFFF);
			draw_lowercase_letter("quit", 20, -20, 0.7f, 0x888888);
			if (pressed(BUTTON_ENTER)) {
				playMusic = false;
				playMusic2 = true;
				random = getRandomNumber(2);
				current_gamemode = GM_EXTRA_GAMEPLAY;
			}
		}
		else {
			draw_lowercase_letter("single player", -80, -10, 0.7f, 0x888888);
			draw_lowercase_letter("multiplayer", 20, -10, 0.7f, 0x888888);
			draw_lowercase_letter("extra mode", -80, -20, 0.7f, 0x888888);
			draw_text("QUIT", 20, -20, 1, 0x48FFFF);
			if (pressed(BUTTON_ENTER)) {
				quit = true;
			}
		}
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
		answer1.isCorrect = false;
		answer1.question = false;
		playMusic3 = false;
		playMusic2 = false;
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
		answer1.isCorrect = false;
		answer1.question = false;
		playMusic3 = false;
		playMusic2 = false;
	}
}
//main method called in win main
internal void simulate_game(Input* input, float dt) {

	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xffffff);
	if (current_gamemode == GM_GAMEPLAY) {
		gameplay(input, dt);
		if (player_1_score >= 5) {
			playMusic1 = false;
			playMusic5 = true;
		}
		if (player_1_score >= 10) {
			current_gamemode = GM_WINSCREEN;
		}
		else if (player_2_score >= 10) {
			current_gamemode = GM_LOSESCREEN;
		}
	}
	else if (current_gamemode == GM_JEOPARDY) {
		jeopardy(input,dt);
	}
	else if (current_gamemode == GM_EXTRA_GAMEPLAY) {
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
	else if (current_gamemode == GM_PAUSE) {
		pauseScreen(input, dt);
	}
	else {
		playMusic = true;
		userUI(input,dt);
	}
}


