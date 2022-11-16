#include <fstream>
#include <iostream>
#include <stdio.h>

using namespace std;

char result[10][20];
char output[30][50];
int counter;
int counterP;
int paragraphs[20];

//Разделяет параграфы по массиву output
void separateParagraphs(char temp[200], int counter) {

	char result[200];
	for (int i = 0; i < counter - 1; i++)
	{
		result[i] = temp[i];
	}

	int i = counter - 1;
	__asm {

		xor esi, esi; //обнуление используемых регистров
		xor edi, edi
		xor ebx, ebx
		xor edx, edx
		xor eax, eax

			;//Загруска адресов массивов
			lea edi, output
			lea esi, result

			mov ebx, -1
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
	
		

		m1 :
			inc ecx;//переходим на следующую строчку двумерного массива
			xor ebx, ebx;//обнуляем счетсчик количества символов	
			jmp checkEnd

		m2 :
			push ecx
			imul ecx, ecx, 50;//умножаем ecx на количество символов в двумерном массиве
			add ecx, ebx;// добавляем к элементу его позицию
			mov al, [esi + edx]
			
			mov[edi + ecx - 1], al; //записываем символ из result в output
			pop ecx
			jmp checkEnd





		checkEnd : ;// проверка на конец массива
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
			mov counter, ecx

	}

}

//Считает количество абзацев
//Возвращает 1, если абзац всего один, и 0, если абзацев больше 1
int countParagraphs(char temp[200], int counter) {
	char result[200];
	for (int i = 0; i < counter - 1; i++)
	{
		result[i] = temp[i];
	}
	int flag = 0;
	int i = counter - 1;
	__asm {

		xor esi, esi; //обнуление используемых регистров
		xor edi, edi
		xor ebx, ebx
		xor edx, edx
		xor eax, eax

			;//Загруска адресов массивов
			lea edi, paragraphs
			lea esi, result

			mov edx, -1
			mov ecx, -1
			xor eax, eax;
			xor ebx, ebx

			strt :
				inc edx
				inc ebx
				cmp[esi + edx], '\n'		;//Проверка на символ перевода строки
				jz m1
				jnz checkEnd

					

			m1 :
				inc ecx
				dec ebx							 ;//Т.К. в ebx хранится на 1 символ больше (символ переноса строки), уменьшаем его значение на 1
				mov[edi + ecx * 4], ebx			;//Записываем количество символов в массив paragraphs
				mov ebx, 0
				cmp edx, i
				jz ending
				jnz strt

			

			checkEnd :
				cmp edx, i			;//Проверка на конец массива символов
				jz m1
				jnz strt


			ending :
				inc ecx
				mov counterP, ecx		;// сохраняем в переменную количество абзацев
				cmp counterP, 1
				jz onlyOneP
				jnz e

			onlyOneP :				;//Если абзац только один, то устанавливаем флаг (не требуются перестановки)
				inc flag

			e :
	}
	for (int i = 0; i < counterP; i++)
	{
		cout << i + 1 << " Абзац: " << paragraphs[i] << " символа(ов)\n" <<"Текст: " << output[i] << endl;
	}
	return flag;


}

//Сортировка пузырьком
void asmBubbleSort() {

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
	printf("\n");
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
	counter = 0;
	char ch, temp[200];
	inputFile.seekg(0); //Считываем символы с 0 позиции файла
	while (inputFile) {
		inputFile.get(ch);
		temp[counter] = ch;
		counter++;
		if (counter >= 200)
		{
			printf("Слишком много символов");
			return 0;
		}
	}
	if (counter == 1)
	{
		inputFile.close();
		cout << "Файл пуст" << endl;
		return 0;
	}
	inputFile.close();
	char result[200];
	for (int i = 0; i < counter - 1; i++)
	{
		result[i] = temp[i];
	}
	cout << "Данные из файла:" << endl;
	separateParagraphs(temp, counter);
	l = countParagraphs(temp, counter);
	if (l != 1)
	{
		asmBubbleSort();
	}
	cout << "Отсортированные данные:" << endl;
	ofstream outputFile("output.txt"); //запись в файл
	int k=1;
	for (int i = 0; i < counterP; i++)
	{
		if (strlen(output[i]) != 0)
		{
			cout << k << " Абзац: " << output[i] << "\n" << "Символов " << paragraphs[i] << "\n" << endl;
			outputFile << output[i] << endl;
			k++;
		}
	}
	outputFile.close();
	cout << "Отсортированные данные сохранены в файле output.txt" << endl;
	system("pause");
	return 0;
}
