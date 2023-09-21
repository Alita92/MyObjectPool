#pragma once
#include <stack>
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

using namespace std;

template<typename T>
class JWObjectPool
{
private:
    std::stack<T*> _block_list;     // 풀의 블록마다의 주소값을 저장해 논 스택
    int _block_size;                // 메모리 블록 하나의 지정된 사이즈
    vector<void*> _mem_list;       // 메모리 요구치가 기존 설정한 사이즈보다 많아 질 시 추가 malloc 위한 배열
    int _cur_usage_index;
    
public:
    JWObjectPool(int size = 100)
    {
        _cur_usage_index = 0;
        _block_size = size;

        MakeBlock();
    }

    // 소멸자 단계에서는 모든 풀을 탐색하여 동적 할당 메모리를 소멸 시킵니다.
    ~JWObjectPool()
    {
        if (_cur_usage_index != 0)
        {
            for (int index = 0; index < _cur_usage_index; ++index)
            {
                unsigned int tmp_address = reinterpret_cast<unsigned int>(_mem_list[0]) + sizeof(T) * index;
                T* delete_object = reinterpret_cast<T*>(tmp_address);
                delete_object->~T();
            }
        }

        for (auto mem : _mem_list)
        {
            free(mem);
        }

        _block_size = 0;
    }

    void MakeBlock()
    {
        void* start_point = malloc(sizeof(T) * _block_size);

        for (int index = 0; index < _block_size; ++index)
        {
            unsigned int tmp_address = reinterpret_cast<unsigned int>(start_point) + sizeof(T) * index;
            _block_list.push(reinterpret_cast<T*>(tmp_address));
        }

        _mem_list.push_back(start_point);
    }

    // 풀에서 오브젝트를 꺼내 사용합니다. 
    T* PopObject()
    {
        if (_block_list.empty())
        {
            printf("기존 메모리 블록 할당량을 모두 소비해 새 메모리 블록을 만듭니다!\n");
            MakeBlock();
        }

        T* return_pointer = _block_list.top();
        _block_list.pop();

        T* return_pointer_placement = new (&return_pointer) T;
        // PLACEMENT NEW

        ++_cur_usage_index;
        return return_pointer_placement;
    }

    // 오브젝트를 수거한다.
    void ReturnObject(T* object)
    {
        object->~T();
        _block_list.push(object);
        --_cur_usage_index;
    }
};


class CharacterData
{
public:
    CharacterData() 
    {
        _level = 1;
        _type = 'A';
    }
    ~CharacterData()
    {
        int a = 0;
    }

public:

private:
    int _level;
    char _type;
};

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
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

    return 0;
}