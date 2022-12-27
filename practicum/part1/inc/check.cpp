void check_access() 
{
    lnh_del_str_sync(TEST_STRUCTURE);
    
    //Объявление переменных
    unsigned int count = mq_receive();
    unsigned int size_in_bytes = 2*count*sizeof(uint64_t);
    
    //Создание буфера для приема пакета
    uint64_t *buffer = (uint64_t*)malloc(size_in_bytes);
    
    //Чтение пакета в RAM
    buf_read(size_in_bytes, (char*)buffer);
    for (int i = 0; i < count; i++)
    {
        uint64_t key = buffer[i*2];
        uint64_t value = buffer[i*2 + 1];
        
        // Поиск ключа key в структуре TEST_STRUCTURE и выдача найденного ключа и значения value.
        //      res = false   - ключ ненайден
        //      res = true    - ключ найден
        bool res = lnh_search(TEST_STRUCTURE, key);
        if (!res) {
            //Вставка ключа key и значения value в структуру TEST_STRUCTURE. Запись результата в очередь.
            lnh_ins_syncq(TEST_STRUCTURE, key, value);
            mq_send(lnh_core.result.status);
        } else if (value == lnh_core.result.value) {
            mq_send(1);
        } else if (value != lnh_core.result.value) {
            mq_send(0);
        }
    }
    lnh_sync();
    free(buffer);
}

...
	__foreach_core(group, core) {
		// Пары key - value
		host2gpc_buffer[group][core][0] = 1;
		host2gpc_buffer[group][core][1] = 2;
		host2gpc_buffer[group][core][2] = 1;
		host2gpc_buffer[group][core][3] = 2;
		host2gpc_buffer[group][core][4] = 5;
		host2gpc_buffer[group][core][5] = 6;
		host2gpc_buffer[group][core][6] = 7;
		host2gpc_buffer[group][core][7] = 8;

		host2gpc_buffer[group][core][8] = 1;
		host2gpc_buffer[group][core][9] = 2;
		host2gpc_buffer[group][core][10] = 3;
		host2gpc_buffer[group][core][11] = 4;
		host2gpc_buffer[group][core][12] = 5;
		host2gpc_buffer[group][core][13] = 5;
		host2gpc_buffer[group][core][14] = 7;
		host2gpc_buffer[group][core][15] = 7;
	}

	__foreach_core(group, core) {
		lnh_inst.gpc[group][core]->start_async(__event__(check_access));
	}

		//DMA запись массива host2gpc_buffer в глобальную память
	__foreach_core(group, core) {
		lnh_inst.gpc[group][core]->buf_write(BURST*2*sizeof(uint64_t),(char*)host2gpc_buffer[group][core]);
	}

	//Ожидание завершения DMA
	__foreach_core(group, core) {
		lnh_inst.gpc[group][core]->buf_write_join();
	}



	// Получить сигналы
	unsigned int signals[LNH_GROUPS_COUNT][LNH_MAX_CORES_IN_GROUP][BURST];
	printf("Полученные сигналы:\n");
	__foreach_core(group, core) {
		lnh_inst.gpc[group][core]->mq_send(8);
	}

	__foreach_core(group, core) {
		for (int i = 0; i < BURST; i++) {
			signals[group][core][i] = lnh_inst.gpc[group][core]->mq_receive();
			printf("Group: %d, Core: %d, signal: %d\n", group, core, signals[group][core][i]);
		}
	}

	bool error = false;
	// Проверка  данных
	// NOTE: 151062035 - код статуса
	__foreach_core(group, core) {
		error = signals[group][core][0] != 151062035 ||
				signals[group][core][1] != 1		 ||
				signals[group][core][2] != 151062035 ||
				signals[group][core][3] != 151062035 ||
				signals[group][core][4] != 1 ||
				signals[group][core][5] != 151062035 ||
				signals[group][core][6] != 0 ||
				signals[group][core][7] != 0;
				
	}

	__foreach_core(group, core) {
		free(host2gpc_buffer[group][core]);
	}


	if (!error)
		printf("Тест пройден успешно!\n");
	else
		printf("Тест завершен с ошибкой!\n");

...