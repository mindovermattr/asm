#include <fstream>	//Для работы с файлами
#include <iostream>	//Для работы с потоковым вводом/выводом
#include <windows.h>
#include <chrono>	//Для тестирование быстроты выполнения

using namespace std;


//Разделяет параграфы по массиву output
// В функцию передается указатель на массив символов из файла, 
//Возвращает количество абзацев, или 0, если в одном из абзацев более 150 символов
int separateParagraphs(char* temp1, int counter, int* paragraphs, char output[500][400]) {
	int counterP;
	int i = counter - 1;
	__asm {

		xor esi, esi; //обнуление используемых регистров
		xor edi, edi
		xor edx, edx


			;//Загруска адресов массивов
		mov edi, output
		mov esi, temp1;//Т.к. в функцию передаются адрес можно не загружать его при помощи lea, а взять сразу 

		mov ebx, 0
		mov edx, -1
		mov ecx, 0
		xor eax, eax;
		xor ebx, ebx

	strt :
		inc edx
		inc ebx
		cmp[esi + edx], '\n';//проверка на абзац. Если есть символ переноса строки нет, то записываем символ из массива result в output, 
	;// иначе берем следующую ячейку массива output и складываем туда символы до символа переноса строки
		jz m1
		jnz m2


			;//Проверка на абзац без символов,запись количества символов в массив и переход на следующую строчку двумерного массива и массива типа int
	m1:
		;//Проверка на абзац без символов
		cmp[esi + edx + 1], '\n'
			jz oneLetter
			cmp ebx, 1;//Проверка на 1 символ в абзаце меньше или равен- переход
		jbe oneLetter

	addParagraph :
		push esi;//Ложим в стек адрес, чтобы его не потерять
			mov esi, paragraphs
			dec ebx;//В ebx всегда хранится на 1 символ больше (учитывается символ переноса строки), поэтому уменьшаем его значение на 1
			mov[esi + ecx * 4], ebx;//записываем количество символов в массив paragraphs
		pop esi;//Возвращаем значение адреса массива символов temp1
		xor ebx, ebx;//обнуляем счетчик количества символов	
		inc ecx;//переходим на следующую строчку двумерного массива	
		jmp checkEnd

			;//Запись элемента в двумерный массив output
	m2:
		push ecx;//Сохраняем значение ecx
			imul ecx, ecx, 400;//умножаем ecx на количество символов в одной строчке двумерного массива
			add ecx, ebx;// добавляем к элементу его позицию
			mov al, [esi + edx]
			mov[edi + ecx - 1], al; //записываем символ из result в output
		pop ecx;//Возвращаем значение ecx
		jmp checkEnd


	oneLetter : ;//проверка: есть ли в абзаце символы, кроме \n, чтобы не записывать в массив абзац без символов
		cmp ebx, 1
		ja addParagraph;	//Если больше 1, то добавляем информацию о количестве символов
		xor ebx, ebx
		jmp checkEnd


	checkEnd : ;// проверка на конец массива
		cmp edx, i
		jz ending
		jnz strt

			;//конец
	ending:
		push ecx
			imul ecx, ecx, 400
			add ecx, ebx
			mov[edi + ecx - 1], '\0';//замена последнего элемента массива 
		pop ecx

		mov esi, paragraphs;//Добавляется информация о количестве символов последнего абзаца
		cmp ebx, 0;//В последнем абзаце не было символов? Если да, то записываем количество абзацев в переменную, иначе - записываем количество символов и количество абзацеев 
		jz nextStep
		dec ebx
		mov[esi + ecx * 4], ebx
		inc ecx

	nextStep :
		mov counterP, ecx

	}

	if (counterP > 500) {
		cout << "В файле более 500 абзацев. Будут взяты только первые 500" << endl;
		counterP = 500;
	}

	for (int i = 0; i < counterP; i++)
	{

		cout << "\n" << i + 1 << " Абзац:" << output[i] << endl;
		cout << "Количество символов: " << paragraphs[i] << endl;
		if (paragraphs[i] > 400) {
			cout << "В абзаце "<<i+1<<" более 400 символов. Сортировка не будет выполнена" << endl;
			return 0;
		}
	}

	return counterP;

}

//Сортировка методом пузырька абзацев по количеству символов. Реализована на ассемблере
//В Процедуру необходимо передать двумерный массив символов, количество абзацев, указатель на массив, состоящий из количества символов абзацев
//Процедура ничего не возвращает
void asmBubbleSort(char output[500][400], int counterP, int* paragraphs) {

	int i = counterP - 1;

	__asm {
		mov esi, paragraphs;// позиционируемся на массив
		mov edi, output

			strt :
		mov ecx, i;//загружаем счетчик
		xor ebx, ebx;// флаг – были / не были перестановки в проходе

	checker:
		mov eax, [esi + ecx * 4 - 4];// получаем значение очередного элемента
		cmp[esi + ecx * 4], eax;// сравниваем со значением соседнего элемента
		jnb checkEnd;// если больше или равен - идем к следующему элементу
		setna bl;// была перестановка - взводим флаг
		xchg eax, [esi + ecx * 4];// меняем значение элементов местами
		mov[esi + ecx * 4 - 4], eax

			;//Меняем значения массива output исходя из значений массива paragraphs
		xor edx, edx
			push ecx
			imul ecx, ecx, 400

			;//Меняем значение каждого символа
	cngOutpt:
		mov al, [edi + ecx - 1]
			xchg[edi + ecx + 399], al
			mov[edi + ecx - 1], al
			inc edx
			cmp edx, 400
			loopnz cngOutpt
			pop ecx



		checkEnd:
		loop checker; //двигаемся вверх до границы массива
		add esi, 4;// сдвигаем границу отсортированного массива
		add edi, 400
			dec ebx;// проверяем были ли перестановки
		jnz ending;// если перестановок не было - заканчиваем сортировку
		dec i;// уменьшаем количество неотсортированных элементов
		jnz strt;// если есть еще неотсортированные элементы - начинаем новый проход

		;//Выход из ассемблера
	ending:

	}

}

//Функция чтения из файла
//Как параметры передаются массив для символов, адрес для записи числа символов из файла
//Возвращает true в случае успеха, false в случае неудачи.
bool ReadFromFile(char* temp, int* a) {
	fstream inputFile("input.txt");//создание потока для чтения из файла
	if (!inputFile.is_open()) {

		cout << "Не удалось открыть файл" << endl;
		return false;
	}

	int counter = 0; //счетчик символов
	int counterN = 0;//счетчик символов переноса строки
	char ch;//символьная переменная

	inputFile.seekg(0); //Считываем символы с 0 позиции файла
	while (inputFile) {
		inputFile.get(ch);
		temp[counter] = ch;
		counterN += ch == '\n' ? 1 : 0;
		counter++;
		if (counter > 10000)			//Если в файле больше символов, чем может вместить массив - выход
		{
			cout << "Символов больше 10000,что превышает размер массива символов, поэтому для сортировки будут использованы только первые 10000" << endl;
			break;
		}
	}
	//Если файл пустой, то вывести соответствующее сообщение
	if (counter == 1 || (counterN == counter))
	{
		inputFile.close();
		cout << "Файл пуст" << endl;
		return false;
	}

	inputFile.close();

	*a = counter;

	return true;
}

//Процедура для записи в файл
//В процедуру передается количество абзацев,двумерный массив символов, массив количества символов
void WriteInFile(int counterParagraphs, char output[500][400], int paragraphs[500]) {

	ofstream outputFile("output.txt"); //запись в файл

	//Вывод на экран отсортированных данных
	for (int i = 0; i < counterParagraphs; i++)
	{

		cout << i + 1 << " Абзац: " << output[i] << "\n" << "Символов " << paragraphs[i] << "\n" << endl;
		outputFile << output[i] << endl;

	}

	outputFile.close();

}

int main()
{
	auto start_time = std::chrono::steady_clock::now();
	int l, counter;
	//SetConsoleCP(1251);
	//SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian"); //Русский язык в консоли

	char temp[10000] = { 0 }, output[500][400] = { 0 };

	//Если не удалось прочитать из файла символы или файл пустой - выход из программы
	if (!ReadFromFile(temp, &counter)) return 0;

	int paragraphs[500] = { 0 };

	cout << "***************Данные из файла:****************" << endl;

	l = separateParagraphs(temp, counter, paragraphs, output);
	//Если абзацев более одного, то сортируем
	switch (l)
	{
		case 0:
			return 0;	//В одном из абзаце более 400 символов 
			break;
		case 1:
			cout << "В файле только 1 абзац" << endl;
			break;
	default:
		asmBubbleSort(output, l, paragraphs);
		break;
	}

	cout << "***************Отсортированные данные:****************" << endl;
	WriteInFile(l, output, paragraphs);
	cout << "Отсортированные данные сохранены в файле output.txt\n" << endl;


	auto end_time = std::chrono::steady_clock::now();
	auto elapsed_ns = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	//cout << "Время работы программы:" << elapsed_ns.count() << " ms\n";
	
	system("pause");
	return 1;
}
