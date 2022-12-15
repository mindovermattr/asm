#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cstring>

using namespace std;
char output[30][50], tmp[30][50];
int paragraphs[30];





//Разделяет параграфы по массиву output
//Возвращает количество абзацев
int separateParagraphs(char* temp1,int counter) {
	int counterP;
	int i = counter - 1;
	__asm {

		xor esi, esi; //обнуление используемых регистров
		xor edi, edi
		xor edx, edx
			

			;//Загруска адресов массивов
			lea edi, output
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
		cmp [esi+edx+1], '\n'
		jz oneLetter
		cmp ebx,1
		jbe oneLetter
			addParagraph:
				push esi;//Ложим в стек адрес, чтобы его не потерять
				lea esi, paragraphs
				dec ebx;//В ebx всегда хранится на 1 символ больше (учитывается символ переноса строки), поэтому уменьшаем его значение на 1
				mov[esi + ecx * 4], ebx;//записываем количество символов в массив paragraphs
				pop esi;//Возвращаем значение адреса массива символов temp1
				xor ebx, ebx;//обнуляем счетчик количества символов	
				inc ecx;//переходим на следующую строчку двумерного массива	
		jmp checkEnd

			;//Запись элемента в двумерный массив output
			m2 :
				push ecx;//Сохраняем значение ecx
				imul ecx, ecx, 50;//умножаем ecx на количество символов в одной строчке двумерного массива
				add ecx, ebx;// добавляем к элементу его позицию
				mov al, [esi + edx]
				mov[edi + ecx - 1], al; //записываем символ из result в output
				pop ecx;//Возвращаем значение ecx
				jmp checkEnd


			oneLetter :				;//проверка на символ переноса строки, чтобы не записывать его.
				cmp ebx,1
				ja addParagraph;	//Если больше 1, то добавляем информацию о количестве символов
				xor ebx,ebx
				jmp checkEnd


		checkEnd:;// проверка на конец массива
			cmp edx, i
			jz ending
			jnz strt

			;//конец
	ending:
		push ecx
			imul ecx, ecx, 50
			add ecx, ebx
			mov[edi + ecx - 1], '\0';//замена последнего элемента массива 
		pop ecx

			lea esi, paragraphs;//Добавляется информация о количестве символов последнего абзаца
			mov[esi + ecx * 4], ebx
			mov counterP, ecx
			
	}

	int f = 0;
	for (int i = 0; i <=counterP; i++)
	{
		
		cout << "\n" << i + 1 << " Абзац:" << output[i] << endl;
		cout << "Количество символов: " << paragraphs[i] << endl;
		_asm {

		}
	}
	int flag = 0, tempCounterP = counterP;
	
	
	cout << "Без пустых строк (за пустые строки считаются те строки, которые не содержат ни 1 символ)" << endl;
	for (int i = 0; i <= counterP; i++)
	{
		
		cout << "\n" << i + 1 << " Абзац:" << output[i] << endl;
		cout << "Количество символов: " << paragraphs[i] << endl;
	}
	return counterP;

}

//Сортировка методом пузырька абзацев. Реализована на ассемблере
//В функцию необходимо передать количество абзацев
void asmBubbleSort(int counterP) {

	int i = counterP - 1;

	__asm {
		lea esi, paragraphs;// позиционируемся на массив
		lea edi, output

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
			imul ecx, ecx, 50

			cngOutpt:
		mov al, [edi + ecx - 1]
			xchg[edi + ecx + 49], al
			mov[edi + ecx - 1], al
			inc edx
			cmp edx, 50
			loopnz cngOutpt
			pop ecx



		checkEnd:
		loop checker; //двигаемся вверх до границы массива
		add esi, 4;// сдвигаем границу отсортированного массива
		add edi, 50
			dec ebx;// проверяем были ли перестановки
		jnz ending;// если перестановок не было - заканчиваем сортировку
		dec i;// уменьшаем количество неотсортированных элементов
		jnz strt;// если есть еще неотсортированные элементы - начинаем новый проход

		;//Выход из ассемблера
	ending:

	}

}

int main()
{
	int l;
	setlocale(LC_ALL, "Russian");
	fstream inputFile("input.txt");
	if (!inputFile.is_open()) {
		cout << "Не удалось открыть файл" << endl;
		return 0;
	}
	int counter = 0;
	char ch, temp[256];
	inputFile.seekg(0); //Считываем символы с 0 позиции файла
	while (inputFile) {
		inputFile.get(ch);
		temp[counter] = ch;
		counter++;
		if (counter >= 256)			//Если в файле больше символов, чем может вместить массив - выход
		{
			inputFile.close();
			cout << "Символов больше 256, поэтому берутся только первые 256" << endl;
			break;
		}
	}
	//Проверка файла на пустоту
	if (counter == 1)
	{
		inputFile.close();
		cout << "Файл пуст" << endl;
		return 0;
	}
	inputFile.close();

	cout << "***************Данные из файла:****************" << endl;
	l = separateParagraphs(temp,counter);
	if (l != 1) asmBubbleSort(l);

	cout << "\n\n**************Отсортированные данные:*****************" << endl;

	ofstream outputFile("output.txt"); //запись в файл



	for (int i = 0; i < l; i++)
	{

		cout << i+1 << " Абзац: " << output[i] << "\n" << "Символов " << paragraphs[i] << "\n" << endl;
		outputFile << output[i] << endl;

	}

	outputFile.close();

	cout << "Отсортированные данные сохранены в файле output.txt\n" << endl;

	system("pause");
	return 0;
}
