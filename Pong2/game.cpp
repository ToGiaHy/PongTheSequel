#include <string>
#include<cstdlib>
#include <map>
#include <vector>

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

//float player_pos_x = 0.f;
float player_1_p, player_1_dp, player_2_p, player_2_dp, player_3_p = 0, player_3_dp;
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
	GM_QUESTION,
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
int countLong;
int speed;

struct Answer {
	bool question;   // Text representing the answer option
	bool isCorrect;     // Indicates whether the answer is correct or not

	// Constructor to initialize the Answer struct
	Answer(bool question, bool isCorrect) : question(question), isCorrect(isCorrect) {}
};
Answer answer1(false, false);
std::map<int, Question> questionAlgo = {
		{0, {"THIS GAME IS CALLED PONG ?", {"YES", "NO", "MAYBE"}, "YES"}},
		{1, {"THERE IS NOTHING WE CAN DO ?", {"YES", "NO", "MAYBE"}, "NO"}},
		{2, {"YOU ARE DEAD INSIDE ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{3, {"HOW MANY ?", {"YES", "NO", "MAYBE"}, "MAYBE"}}
};
std::map<int, Question> questionAlgo1 = {
		{0, {"THIS GAME IS CALLED PONG ?", {"YES", "NO", "MAYBE"}, "YES"}},
		{1, {"THERE IS NOTHING WE CAN DO ?", {"YES", "NO", "MAYBE"}, "NO"}}
};
std::map<int, Question> questionAlgo2 = {
		{0, {"YOU ARE DEAD INSIDE ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{1, {"HOW MANY ?", {"YES", "NO", "MAYBE"}, "MAYBE"}}
};
std::map<int, Question> questionSEPM = {
		{0, {"THIS GAME IS CALLED PONG ?", {"YES", "NO", "MAYBE"}, "YES"}},
		{1, {"THERE IS NOTHING WE CAN DO ?", {"YES", "NO", "MAYBE"}, "NO"}},
		{2, {"YOU ARE DEAD INSIDE ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{3, {"HOW MANY ?", {"YES", "NO", "MAYBE"}, "MAYBE"}}
};
std::map<int, Question> questionBITS = {
		{0, {"THIS GAME IS CALLED PONG ?", {"YES", "NO", "MAYBE"}, "YES"}},
		{1, {"THERE IS NOTHING WE CAN DO ?", {"YES", "NO", "MAYBE"}, "NO"}},
		{2, {"YOU ARE DEAD INSIDE ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{3, {"HOW MANY ?", {"YES", "NO", "MAYBE"}, "MAYBE"}}
};
std::map<int, Question> questionLeadership = {
		{0, {"THIS GAME IS CALLED PONG ?", {"YES", "NO", "MAYBE"}, "YES"}},
		{1, {"THERE IS NOTHING WE CAN DO ?", {"YES", "NO", "MAYBE"}, "NO"}},
		{2, {"YOU ARE DEAD INSIDE ?", {"YES", "NO", "MAYBE"}, "MAYBE"}},
		{3, {"HOW MANY ?", {"YES", "NO", "MAYBE"}, "MAYBE"}}
};

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
		draw_text(questionText, -80, 20, 0.6, 0xFFFF00);

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
internal void simulate_game(Input* input, float dt) {

	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xffffff);
	current_gamemode = GM_GAMEPLAY;
	if (current_gamemode == GM_GAMEPLAY) {
		if (player_1_score == 5) {
			testQuestion(input, random);
			if (answer1.question == true) {
				gameplay(input, dt);
			}
		}
		else {
			gameplay(input, dt);
		}
	}
	else if (current_gamemode == GM_EXTRA_GAMEPLAY) {
	}
}