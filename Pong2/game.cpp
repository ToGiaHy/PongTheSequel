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