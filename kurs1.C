#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cstring>

using namespace std;

char result[10][20];
char output[30][50],tmp[30][50];
int counter;
int counterP;
int paragraphs[30];
char temp1[256],q[256];


int qwe(int counter) {
	_asm {
		xor esi, esi; //обнуление используемых регистров
		xor edi, edi
		xor ebx, ebx
		xor edx, edx
		xor eax, eax
		mov ecx,counter
			;//Загруска адресов массивов
		lea edi, output
		lea esi, q
			strt:
		mov al, [edi+ebx]
		mov [esi+ebx],al
		inc ebx
		cmp al, ' '
		jz voidSym
		cmp al, '\t'
		jz voidSym
		cmp al, '\0'
		jz check
		xor ebx,ebx
		xor edx,edx
		add edi,50
		loop strt

		voidSym:
			inc edx
			jmp strt

		check:
			dec ebx
			cmp ebx,edx
			jz voidLine
			push ecx
			mov ecx,ebx
			
			
			loop1:
			mov al,[esi+ecx]
			mov [edi+ecx],al
			loop loop1
		
			pop ecx
			add edi,50
			xor ebx,ebx
			xor edx,edx
			loop strt
	
		voidLine:
			xor ebx, ebx
				add edi, 50
			xor edx, edx
			loop strt

	}
	printf("asdasd");
}

//Разделяет параграфы по массиву output
//Возвращает количество абзацев
int separateParagraphs( int counter) {

	int i = counter - 1;
	__asm {

			xor esi, esi; //обнуление используемых регистров
			xor edi, edi
			xor ebx, ebx
			xor edx, edx
			xor eax, eax

			;//Загруска адресов массивов
			lea edi, output
			lea esi, temp1

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



		m1:
			push esi;//Ложим в стек адрес, чтобы его не потерять
			lea esi, paragraphs
			mov[esi + ecx * 4], ebx;//
			pop esi
			xor ebx, ebx;//обнуляем счетсчик количества символов	
			inc ecx;//переходим на следующую строчку двумерного массива	
			jmp checkEnd

		m2 :
			push ecx
			imul ecx, ecx, 50;//умножаем ecx на количество символов в двумерном массиве
			add ecx, ebx;// добавляем к элементу его позицию
			mov al, [esi + edx]
			mov[edi + ecx - 1], al; //записываем символ из result в output
			pop ecx
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
			lea esi, paragraphs
			mov [esi+ecx*4],ebx
			mov counterP, ecx
			inc counterP
	}

	int f=0;
	for (int i = 0; i < counterP; i++)
	{
		paragraphs[i] -= 1;
		if (paragraphs[i] != 0) {
			strcpy(tmp[f], output[i]);
			f++;
		};
		cout<<"\n" << i + 1 << " Абзац:" << output[i] << endl;
		cout << "Количество символов: " << paragraphs[i] << endl;
	}
	int flag = 0,tempCounterP=counterP;
	for (int i = 0; i < counterP; i++)
	{
		flag = 0;
		strcpy(output[i],tmp[i]);
		_asm {
			xor edx,edx
			lea edi, output

			lea esi, paragraphs
			xor ebx,ebx
			mov ecx,i
			imul edx, ecx,50
			strt1:
			mov al,[edi+edx]
			inc edi
			inc ebx
			cmp al, 0
			jz write
			jmp strt1

				write:
			mov [esi+ecx*4], ebx
			cmp ebx, 1
			jnz ex
			inc flag
		
				
			ex:
		}
		if (flag == 1) tempCounterP -= 1;		

	}
	_asm {
		mov eax, tempCounterP
		mov counterP,eax
	}
	cout << "Без пустых строк (за пустые строки считаются те строки, которые не содержат ни 1 символ)" << endl;
	for (int i = 0; i < counterP; i++)
	{
		paragraphs[i] -= 1;
		cout << "\n" << i + 1 << " Абзац:" << output[i] << endl;
		cout << "Количество символов: " << paragraphs[i] << endl;
	}
	return counterP;

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
			cmp[esi + edx], '\n';//Проверка на символ перевода строки
		jz m1
			jnz checkEnd



		m1:
			cmp[esi + edx+1], '\n';
			jz checkEnd

			inc ecx
			dec ebx;//Т.К. в ebx хранится на 1 символ больше (символ переноса строки), уменьшаем его значение на 1
			mov[edi + ecx * 4], ebx;//Записываем количество символов в массив paragraphs
			mov ebx, 0
			cmp edx, i
			jz ending
			jnz strt



		checkEnd:
			cmp edx, i;//Проверка на конец массива символов
			jz m1
			jnz strt


			ending :
		inc ecx
			mov counterP, ecx;// сохраняем в переменную количество абзацев
		cmp counterP, 1
			jz onlyOneP
			jnz e

			onlyOneP : ;//Если абзац только один, то устанавливаем флаг (не требуются перестановки)
		inc flag

			e :
	}
	for (int i = 0; i < counterP; i++)
	{
		cout << i + 1 << " Абзац: " << paragraphs[i] << " символа(ов)\n" << "Текст: " << output[i] << endl;
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
	char ch, temp[256];
	inputFile.seekg(0); //Считываем символы с 0 позиции файла
	while (inputFile) {
		inputFile.get(ch);
		temp[counter] = ch;
		counter++;
		if (counter >= 256)			//Если в файле больше символов, чем может вместить массив - выход
		{
			inputFile.close();
			printf("Символов больше 256, поэтому берутся только первые 256");
			break;
		}
	}
	for (int i = 0; i < counter - 1; i++)
	{
		temp1[i] = temp[i];
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
	l=separateParagraphs(counter);
//	qwe(l);
	//l = countParagraphs(temp, counter);
	if (l != 1) asmBubbleSort();

	cout << "\n\n**************Отсортированные данные:*****************" << endl;

	ofstream outputFile("output.txt"); //запись в файл

	int k = 1;

	for (int i = 0; i < counterP; i++)
	{
		
		cout << k << " Абзац: " << output[i] << "\n" << "Символов " << paragraphs[i] << "\n" << endl;
		outputFile << output[i] << endl;
		k++;
		
	}

	outputFile.close();

	cout << "Отсортированные данные сохранены в файле output.txt\n" << endl;

	system("pause");
	return 0;
}
