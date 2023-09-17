#pragma once
#include <stack>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;

template<typename T>
class JWObjectPool
{
private:
	std::stack<T*> _pool;
	int _maxSize;

public:
	// 생성자 단계에서 최대 사이즈를 정해 생성합니다. 기본 값은 100입니다.
	JWObjectPool(int size = 100)
	{
		_maxSize = size;

		for (int index = 0; index < _maxSize; ++index)
		{
			T* newObject = new T();
			_pool.push(newObject);
			printf("%d 번째 오프젝트 풀 원소 생성\n", index + 1);
		}
	}

	// 소멸자 단계에서는 모든 풀을 탐색하여 동적 할당 메모리를 소멸 시킵니다.
	~JWObjectPool()
	{
		while (_pool.empty() == false)
		{
			T* object = _pool.top();
			_pool.pop();
			delete object;
			object = nullptr;
		}

		printf("오브젝트 풀 원소 모두 삭제 완료\n");
		_maxSize = 0;
	}

	// 풀에서 오브젝트를 꺼내 사용합니다. 
	T* PopObject()
	{
		// 오브젝트 풀이 비어 있는데 pop 요청이 들어오면, 기존 캐퍼시티의 두 배만큼의 캐퍼시티로 새로 확장합니다.
		if (_pool.empty())
		{
			for (int i = 0; i < _maxSize; ++i)
			{
				T* object = new T();
				_pool.push(object);
			}

			_maxSize *= 2;
			printf("풀 내 원소를 모두 사용 중입니다. 새로 캐퍼시티를 할당합니다.\n현재 풀 사이즈 : %d\n", _maxSize);
		}

		T* popped_object = _pool.top();
		delete _pool.top();

		_pool.pop();
		printf("풀에서 오브젝트를 가져옵니다.. 남은 풀 사이즈 : %d\n", _pool.size());
		return popped_object;
	}

	// 오브젝트를 수거한다.
	void ReturnObject(T* object)
	{
		_pool.push(object);
		printf("오브젝트를 풀로 반환합니다.. 현재 풀 사이즈 : %d\n", _pool.size());
	}
};


class CharacterData 
{
public:
	CharacterData() {}

private :
	int _level = 1;
	float _hp = 100.f;
	char _type = 'A';
};

int main() 
{
		JWObjectPool<CharacterData> pool(10);

		bool is_game_running = true;
		// 플레이 세션 도중..
		printf("게임 로딩 완료.\n Q 키를 눌러 게임 종료 \n A 키를 눌러 캐릭터 추가 생성 \n Z 키를 눌러 캐릭터 제거\n");

		stack<CharacterData*> live_character_list;

		while (is_game_running)
		{
			if (_kbhit())
			{
				char key;
				key = _getch();

				switch (key)
				{
				case 'q':
				case 'Q':
					is_game_running = false;
					break;
				case 'a':
				case 'A':
				{
					CharacterData* character = pool.PopObject();
					live_character_list.push(character);
					printf("캐릭터 Init 완료. 게임에서 사용 중인 캐릭터 수 : %d \n", live_character_list.size());
				}
				break;
				case 'z':
				case 'Z':
				{
					if (live_character_list.empty() == false)
					{
						pool.ReturnObject(live_character_list.top());
						live_character_list.pop();
						printf("캐릭터 제거 완료. 게임에서 사용 중인 남은 캐릭터 수 : %d \n", live_character_list.size());
					}
					else
					{
						printf("게임에서 사용 중인 캐릭터가 없어 제거 할 수 없습니다! \n");
					}
				}
				break;
				}
			}
		}

		printf("게임이 종료되어 현재 활성화된 캐릭터를 모두 제거합니다.\n");
		while (live_character_list.empty() == false)
		{
			pool.ReturnObject(live_character_list.top());
			live_character_list.pop();
			printf("남은 활성화된 캐릭터 수 : %d\n", live_character_list.size());
		}

	return 0;
}