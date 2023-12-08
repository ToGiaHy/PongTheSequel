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

//float player_pos_x = 0.f;
float player_1_p, player_1_dp, player_2_p, player_2_dp, player_3_p = 0, player_3_dp, player_3_ddp = 0;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;
float ball_p_x, ball_p_y, ball_dp_x = 130, ball_dp_y, ball_half_size = 1;
float ball_p1_x, ball_p1_y, ball_dp1_x = 130, ball_dp1_y, ball_half1_size = 1;

int player_1_score, player_2_score;

struct Question {
	const char* questionText;
	std::vector<const char*> answerOptions;
	const char* correctAnswer;
};


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

internal bool
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {
	return (p1x + hs1x > p2x - hs2x &&
		p1x - hs1x < p2x + hs2x &&
		p1y + hs1y > p2y - hs2y &&
		p1y + hs1y < p2y + hs2y);
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
};

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

int getRandomNumber(int range) {
	// Seed the random number generator using current time
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// Generate a random number between 0 and 4
	return std::rand() % range; // Modulus 4 ensures numbers between 0 and 3 (inclusive)
}


struct Answer {
	bool question;   // Text representing the answer option
	bool isCorrect;     // Indicates whether the answer is correct or not

	// Constructor to initialize the Answer struct
	Answer(bool question, bool isCorrect) : question(question), isCorrect(isCorrect) {}
};

Answer answer1(false, false);
static std::map<int, Question> questionAlgo = {
		{0, {"THIS GAME IS CALLED PONG ?", {"YES", "NO", "MAYBE"}, "YES"}},
		{1, {"THERE IS NOTHING WE CAN DO ?", {"YES", "NO", "MAYBE"}, "NO"}},
		{2, {"YOU ARE DEAD INSIDE ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{3, {"HOW MANY ?", {"YES", "NO", "MAYBE"}, "MAYBE"}}
};
static std::map<int, Question> questionAlgo1 = {
		{0, {"THIS GAME IS CALLED PONG ?", {"YES", "NO", "MAYBE"}, "YES"}},
		{1, {"THERE IS NOTHING WE CAN DO ?", {"YES", "NO", "MAYBE"}, "NO"}}
};
static std::map<int, Question> questionAlgo2 = {
		{0, {"YOU ARE DEAD INSIDE ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{1, {"HOW MANY ?", {"YES", "NO", "MAYBE"}, "MAYBE"}}
};
static std::map<int, Question> questionSEPM = {
		{0, {"THIS GAME IS CALLED PONG ?", {"YES", "NO", "MAYBE"}, "YES"}},
		{1, {"THERE IS NOTHING WE CAN DO ?", {"YES", "NO", "MAYBE"}, "NO"}},
		{2, {"YOU ARE DEAD INSIDE ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{3, {"HOW MANY ?", {"YES", "NO", "MAYBE"}, "MAYBE"}}
};
static std::map<int, Question> questionBITS = {
		{0, {"THIS GAME IS CALLED PONG ?", {"YES", "NO", "MAYBE"}, "YES"}},
		{1, {"THERE IS NOTHING WE CAN DO ?", {"YES", "NO", "MAYBE"}, "NO"}},
		{2, {"YOU ARE DEAD INSIDE ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{3, {"HOW MANY ?", {"YES", "NO", "MAYBE"}, "MAYBE"}}
};
static std::map<int, Question> questionLeadership = {
		{0, {"THIS GAME IS CALLED PONG ?", {"YES", "NO", "MAYBE"}, "YES"}},
		{1, {"THERE IS NOTHING WE CAN DO ?", {"YES", "NO", "MAYBE"}, "NO"}},
		{2, {"YOU ARE DEAD INSIDE ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{3, {"HOW MANY ?", {"YES", "NO", "MAYBE"}, "MAYBE"}}
};
int pointerX;
int pointerY;
internal void jeopardy(Input* input) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xff5500);
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
			draw_text("ALGO", -80, -10, 1, 0xff0000);
			draw_text("SEPM", 20, -10, 0.7f, 0xaaaaaa);
			draw_text("BITS", -80, -20, 0.7f, 0xaaaaaa);
			draw_text("LEADERSHIP", 20, -20, 0.7f, 0xaaaaaa);
			if (pressed(BUTTON_ENTER)) {
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 1;
			}
		}
		else {
			draw_text("ALGO", -80, -10, 0.7f, 0xaaaaaa);
			draw_text("SEPM", 20, -10, 1, 0xff0000);
			draw_text("BITS", -80, -20, 0.7f, 0xaaaaaa);
			draw_text("LEADERSHIP", 20, -20, 0.7f, 0xaaaaaa);
			if (pressed(BUTTON_ENTER)) {
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 2;
			}
		}
	}
	else if (pointerY == 1) {
		if (pointerX == 0) {
			draw_text("ALGO", -80, -10, 0.7f, 0xaaaaaa);
			draw_text("SEPM", 20, -10, 0.7f, 0xaaaaaa);
			draw_text("BITS", -80, -20, 1, 0xff0000);
			draw_text("LEADERSHIP", 20, -20, 0.7f, 0xaaaaaa);
			if (pressed(BUTTON_ENTER)) {
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 3;
			}
		}
		else {
			draw_text("ALGO", -80, -10, 0.7f, 0xaaaaaa);
			draw_text("SEPM", 20, -10, 0.7f, 0xaaaaaa);
			draw_text("BITS", -80, -20, 0.7f, 0xaaaaaa);
			draw_text("LEADERSHIP", 20, -20, 1, 0xff0000);
			if (pressed(BUTTON_ENTER)) {
				random = getRandomNumber(4);
				current_gamemode = GM_GAMEPLAY;
				questionType = 4;
			}
		}
	}
	if (pressed(BUTTON_N)) {
		current_gamemode = GM_UI;
	}
}


internal void testQuestion(Input* input, int questionIndex, float dt) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xffffff);
	std::map<int, Question> questions;
	if (questionType == 1) {
		questions = questionAlgo;
	}
	else if (questionType == 2) {
		questions = questionSEPM;
	}
	else if (questionType == 3) {
		questions = questionBITS;
	}
	else {
		questions = questionLeadership;
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
		draw_text(questionText, -80, 30, 1, 0xaaaaaa);

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
			draw_text(answerOptions[0], -80, -10, 1, 0xff0000);
			draw_text(answerOptions[1], 20, -10, 1, 0xaaaaaa);
			draw_text(answerOptions[2], -80, -30, 1, 0xaaaaaa);
		}
		else if (display == 2) {
			draw_text(answerOptions[0], -80, -10, 1, 0xaaaaaa); // Display first answer option
			draw_text(answerOptions[1], 20, -10, 1, 0xff0000); // Display second answer option
			draw_text(answerOptions[2], -80, -30, 1, 0xaaaaaa); // Display third answer option
		}
		else {
			draw_text(answerOptions[0], -80, -10, 1, 0xaaaaaa); // Display first answer option
			draw_text(answerOptions[1], 20, -10, 1, 0xaaaaaa); // Display second answer option
			draw_text(answerOptions[2], -80, -30, 1, 0xff0000); // Display third answer option
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

internal void gameplay(Input* input, float dt) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xff5500);
	bool slowMo = false;
	if (player_1_score == 5 && answer1.question == false) {
		countLong = 1;
		slowMo = true;
		testQuestion(input, random, dt);
	}
	float player_1_ddp = 0.f;

		player_1_ddp = (ball_p_y - player_1_p) * 100;
		if (player_1_ddp > 1300) player_1_ddp = 1300;
		if (player_1_ddp < -1300) player_1_ddp = -1300;


	float player_2_ddp = 0.f;
	if (answer1.isCorrect == true) {
		if (is_down(BUTTON_W)) player_2_ddp += 5000;
		if (is_down(BUTTON_S)) player_2_ddp -= 5000;
	}
	else if (slowMo) {
		if (is_down(BUTTON_W)) player_2_ddp += 1000;
		if (is_down(BUTTON_S)) player_2_ddp -= 1000;
	}
	else {
		if (is_down(BUTTON_W)) player_2_ddp += 2000;
		if (is_down(BUTTON_S)) player_2_ddp -= 2000;
	}

	simulate_player(&player_1_p, &player_1_dp, player_1_ddp, dt);
	simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);
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
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);


		if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 80, player_1_p, player_half_size_x, player_half_size_y)
			) {
			ball_p_x = 80 - player_half_size_x - ball_half_size;
			ball_dp_x *= -1;
			//ball_dp_y = player_1_dp *.75f; 
			ball_dp_y = (ball_p_y - player_1_p) * 2 + player_1_dp * .75f;
		}

		if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -80, player_2_p, player_half_size_x, player_half_size_y)
			) {
			ball_p_x = -80 + player_half_size_x + ball_half_size;
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player_2_p) * 2 + player_2_dp * .75f;
		}

		

		if (ball_p_y + ball_half_size > arena_half_size_y) {
			ball_p_y = arena_half_size_y - ball_half_size;
			ball_dp_y *= -1;
		}
		else if (ball_p_y - ball_half_size < -arena_half_size_y) {
			ball_p_y = -arena_half_size_y + ball_half_size;
			ball_dp_y *= -1;
		}
		if (ball_p_x + ball_half_size > arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			player_1_score++;
		}
		if (ball_p_x - ball_half_size < -arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			player_2_score++;
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

	draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xff0000);
	draw_rect(-80, player_2_p, player_half_size_x, player_half_size_y, 0xff0000);
	// Inside your gameplay function
	// Detect collision with player 3 on all sides
	if (answer1.question == true && answer1.isCorrect == false) {
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

		draw_rect(30, player_3_p, player_half_size_x, 17, 0xff0000);
	}
}
// UserUI function
internal void userUI(Input* input) {

	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xffffff);
	draw_text("PONG THE SEQUEL", -80, 30, 1, 0xffffff);
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
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xff0000);
			draw_text("MULTIPLAYER", 20, -10, 1, 0xaaaaaa);
			draw_text("EXTRA", -80, -20, 1, 0xaaaaaa);
			draw_text("TUTORIAL", 20, -20, 1, 0xaaaaaa);
			if (pressed(BUTTON_ENTER)) {
				random = rand() % 4;
				random2 = rand() % 2;
				current_gamemode = GM_JEOPARDY;
				enemy_is_ai = hot_button ? 0 : 1;
			}
		}
		else {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xaaaaaa);
			draw_text("MULTIPLAYER", 20, -10, 1, 0xff0000);
			draw_text("EXTRA", -80, -20, 1, 0xaaaaaa);
			draw_text("TUTORIAL", 20, -20, 1, 0xaaaaaa);
			if (pressed(BUTTON_ENTER)) {
				random = rand() % 4;
				random2 = rand() % 2;
				current_gamemode = GM_MULTIPLAYER;
				enemy_is_ai = hot_button ? 0 : 1;
			}
		}
	}
	else if (up_down == 1) {
		if(hot_button == 0){
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xaaaaaa);
			draw_text("MULTIPLAYER", 20, -10, 1, 0xaaaaaa);
			draw_text("EXTRA", -80, -20, 1, 0xff0000);
			draw_text("TUTORIAL", 20, -20, 1, 0xaaaaaa);
			if (pressed(BUTTON_ENTER)) {
				question = false;
				random = rand() % 4;
				random2 = rand() % 4;
				current_gamemode = GM_EXTRA_GAMEPLAY;
			}
		}
		else {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xaaaaaa);
			draw_text("MULTIPLAYER", 20, -10, 1, 0xaaaaaa);
			draw_text("EXTRA", -80, -20, 1, 0xaaaaaa);
			draw_text("TUTORIAL", 20, -20, 1, 0xff0000);
		}
	}
}


internal void extraGameplay(Input* input, float dt) {
}
internal void simulate_game(Input* input, float dt) {

	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xffffff);
	if (current_gamemode == GM_GAMEPLAY) {
		gameplay(input, dt);
		if (player_1_score >= 10) {
			draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);
			draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xff5500);
		}
		else if (player_2_score >= 10) {
			draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);
			draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xff5500);
		}
	}
	else if (current_gamemode == GM_JEOPARDY) {
		jeopardy(input);
	}
	else if (current_gamemode == GM_EXTRA_GAMEPLAY) {
	}
	else {
		userUI(input);
	}
}

