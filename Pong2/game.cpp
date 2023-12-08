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
internal void testQuestion(Input* input, int questionIndex) {

	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xffffff);

	// Display the question
	draw_text("THIS GAME IS CALLED PONG", -80, 30, 1, 0xaaaaaa);

	if (pressed(BUTTON_G)) {
		display = 1;
	}

	if (pressed(BUTTON_H)) {
		display = 2;
	}

	if (pressed(BUTTON_J)) {
		display = 3;
	}

	if (display == 1) {
		draw_text("YES", -80, -10, 1, 0xff0000); // Display first answer option
		draw_text("NO", 0, -10, 1, 0xaaaaaa); // Display second answer option
		draw_text("MAYBE", 40, -10, 1, 0xaaaaaa); // Display third answer option
	}
	else if (display == 2) {
		draw_text("YES", -80, -10, 1, 0xaaaaaa); // Display first answer option
		draw_text("NO", 0, -10, 1, 0xff0000); // Display second answer option
		draw_text("MAYBE", 40, -10, 1, 0xaaaaaa); // Display third answer option
	}
	else {
		draw_text("YES", -80, -10, 1, 0xaaaaaa); // Display first answer option
		draw_text("NO", 0, -10, 1, 0xaaaaaa); // Display second answer option
		draw_text("MAYBE", 40, -10, 1, 0xff0000); // Display third answer option
	}

	if (pressed(BUTTON_N)) {
		// Check if the player selected the correct answer
		const char* playerChoice;
		if (display == 1) {
			playerChoice = "YES";
		}
		else if (display == 2) {
			playerChoice = "NO";
		}
		else {
			playerChoice = "MAYBE";
		}

		if (playerChoice == "YES") {
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

internal void gameplay(Input* input, float dt) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xffffff);
	float player_1_ddp = 0.f;
	if (!enemy_is_ai) {
		if (is_down(BUTTON_UP)) player_1_ddp += 2000;
		if (is_down(BUTTON_DOWN)) player_1_ddp -= 2000;
	}
	else {
		player_1_ddp = (ball_p_y - player_1_p) * 100;
		if (player_1_ddp > 1300) player_1_ddp = 1300;
		if (player_1_ddp < -1300) player_1_ddp = -1300;
	}

	float player_2_ddp = 0.f;
	if (is_down(BUTTON_W)) player_2_ddp += 2000;
	if (is_down(BUTTON_S)) player_2_ddp -= 2000;

	simulate_player(&player_1_p, &player_1_dp, player_1_ddp, dt);
	simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);


	// Simulate Ball
	{
		ball_p_x += ball_dp_x * dt;
		ball_p_y += ball_dp_y * dt;

		if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 80, player_1_p, player_half_size_x, player_half_size_y)) {
			ball_p_x = 80 - player_half_size_x - ball_half_size;
			ball_dp_x *= -1;
			ball_dp_y = (ball_p_y - player_1_p) * 2 + player_1_dp * .75f;
		}
		else if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -80, player_2_p, player_half_size_x, player_half_size_y)) {
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
		else if (ball_p_x - ball_half_size < -arena_half_size_x) {
			ball_dp_x *= -1;
			ball_dp_y = 0;
			ball_p_x = 0;
			ball_p_y = 0;
			player_2_score++;
		}
	}

	draw_number(player_1_score, -10, 40, 1.f, 0xffffff);
	draw_number(player_2_score, 10, 40, 1.f, 0xffffff);

	// Rendering
	draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);

	draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xffffff);
	draw_rect(-80, player_2_p, player_half_size_x, player_half_size_y, 0xffffff);
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




// UserUI function
internal void userUI(Input* input) {

	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xffffff);
	draw_text("PONG THE SEQUEL", -80, 30, 1, 0xffffff);
	if (pressed(BUTTON_LEFT) || pressed(BUTTON_RIGHT)) {
		hot_button = !hot_button;
	}
	if (pressed(BUTTON_J)) {
		up_down = !up_down;
	}

	if (up_down == 0) {
		if (hot_button == 0) {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xff0000);
			draw_text("MULTIPLAYER", 20, -10, 1, 0xaaaaaa);
			draw_text("EXTRA", -80, -20, 1, 0xaaaaaa);
			draw_text("TUTORIAL", 20, -20, 1, 0xaaaaaa);
		}
		else {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xaaaaaa);
			draw_text("MULTIPLAYER", 20, -10, 1, 0xff0000);
			draw_text("EXTRA", -80, -20, 1, 0xaaaaaa);
			draw_text("TUTORIAL", 20, -20, 1, 0xaaaaaa);
		}
		if (pressed(BUTTON_ENTER)) {
			random = rand() % 4;
			random2 = rand() % 2;
			current_gamemode = GM_GAMEPLAY;
			enemy_is_ai = hot_button ? 0 : 1;
		}
	}
	else {
		if (hot_button == 0) {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xaaaaaa);
			draw_text("MULTIPLAYER", 20, -10, 1, 0xaaaaaa);
			draw_text("EXTRA", -80, -20, 1, 0xff0000);
			draw_text("TUTORIAL", 20, -20, 1, 0xaaaaaa);
		}
		else {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xaaaaaa);
			draw_text("MULTIPLAYER", 20, -10, 1, 0xaaaaaa);
			draw_text("EXTRA", -80, -20, 1, 0xaaaaaa);
			draw_text("TUTORIAL", 20, -20, 1, 0xff0000);
		}
		if (pressed(BUTTON_ENTER)) {
			question = false;
			random = rand() % 4;
			random2 = rand() % 4;
			current_gamemode = GM_EXTRA_GAMEPLAY;
		}
	}
}