#include <chrono>
#include <iostream>
#include <list>
#include <Windows.h>

using namespace std;

struct position
{
	int x;
	int y;
};

int main()
{
	const auto screen_width = 120;
	const auto screen_height = 30;
	
	auto* const screen = new wchar_t[screen_width * screen_height];
	auto* const console_buffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
	SetConsoleActiveScreenBuffer(console_buffer);
	DWORD bytes_written = 0;

	while (1)
	{
		auto is_dead = false;
		auto moves = 0;
		auto score = 0;
		auto pressed_right_old = false;
		auto pressed_left_old = false;
		list<position> snake = { {55,15}, {56,15}, {57,15}, {58,15}, {59,15}, {60, 15} };
		position food = { 33, 22 };
		auto direction = 0;
		auto start_time = chrono::system_clock::now();
		
		while(!is_dead)
		{
			auto t1 = chrono::system_clock::now();
			while (std::chrono::system_clock::now() - t1 < 150ms)
			{
				auto pressed_right = (0x8000 & GetAsyncKeyState('\x27')) != 0;
				auto pressed_left = (0x8000 & GetAsyncKeyState('\x25')) != 0;

				if (pressed_right && !pressed_right_old)
				{
					direction++;
					if (direction == 4)
					{
						direction = 0;
					}
				}

				if (pressed_left && !pressed_left_old)
				{
					direction--;
					if (direction == -1)
					{
						direction = 3;
					}
				}

				pressed_right_old = pressed_right;
				pressed_left_old = pressed_left;
			}
			
			position snake_head;
			
			// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
			switch (direction)
			{
			case 0: 
				snake_head = position{ snake.front().x - 1, snake.front().y };
				break;
			case 1: 
				snake_head = position{ snake.front().x, snake.front().y - 1 };
				break;
			case 2: 
				snake_head = position{ snake.front().x + 1, snake.front().y };
				break;
			case 3: 
				snake_head = position{ snake.front().x, snake.front().y + 1 };
				break;
			}

			is_dead = snake_head.x <= 0 || 
					  snake_head.x >= screen_width || 
					  snake_head.y <= 2 || 
					  snake_head.y >= screen_height;
			
			if (!is_dead)
			{
				for (auto i : snake)
				{
					is_dead = snake_head.x == i.x && snake_head.y == i.y;
					if (is_dead)
					{
						break;
					}
				}
			}

			
			if (!is_dead)
			{
				snake.push_front(snake_head);
				snake.pop_back();
				moves++;
			}

			if (snake_head.x == food.x && snake_head.y == food.y)
			{
				score++;

				for (auto i = 0; i < 5; i++)
				{
					snake.push_back({ snake.back().x, snake.back().y });
				}
				
				auto rand_x = rand() % (screen_width + 1);
				auto rand_y = rand() % (screen_height - 3) + 3;
				
				food = position{rand_x, rand_y};
			}

			for (auto i = 0; i < screen_width * screen_height; i++) {
				screen[i] = L' ';
			}

			for (auto i : snake) {
				screen[i.y * screen_width + i.x] = is_dead ? L'+' : L'O';
			}

			screen[snake.front().y * screen_width + snake.front().x] = is_dead ? L'X' : L'@';

			screen[food.y * screen_width + food.x] = L'o';

			wsprintf(&screen[screen_width + screen_width /2 - 10/2], L"SCORE: %d", score);
			
			if (is_dead)
			{
				wsprintf(&screen[15 * screen_width + 40], L"You died! Press SPACE to play again.");
			}
			
			WriteConsoleOutputCharacter(console_buffer, screen, screen_width * screen_height, { 0,0 }, &bytes_written);
		}

		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
	}

	return 0;
}
