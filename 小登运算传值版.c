#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ���ڿ��Ʋ�ɫ���
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// �ṹ�嶨��
typedef struct {
    char date[20];
    char mode[20];
    int questions;
    int score;
} TestRecord;

#define MAX_RECORDS 100

TestRecord records[MAX_RECORDS];
int recordCount = 0;

// ����¼д���ļ�
void saveRecordToFile(TestRecord* record) {
    FILE *file = fopen("test_records.txt", "a"); // ���ļ���׷��ģʽ��
    if (file == NULL) {
        printf(RED "�޷����ļ���\n" RESET);
        return;
    }

    // д���¼���ļ�
    fprintf(file, "Date: %s\n", record->date);
    fprintf(file, "Mode: %s\n", record->mode);
    fprintf(file, "Questions: %d\n", record->questions);
    fprintf(file, "Score: %d\n", record->score);
    fprintf(file, "\n");

    // �ر��ļ�
    fclose(file);
}

// �����Լ�¼��ӵ��ڴ����鲢д���ļ�
void storeTestRecord(const char* date, const char* mode, int questions, int score) {
    if (recordCount < MAX_RECORDS) {
        strcpy(records[recordCount].date, date);
        strcpy(records[recordCount].mode, mode);
        records[recordCount].questions = questions;
        records[recordCount].score = score;
        recordCount++;

        // ����¼д���ļ�
        saveRecordToFile(&records[recordCount - 1]);
    } else {
        printf(RED "�޷���Ӹ����¼������������\n" RESET);
    }
}

// ��ʾ���в��Լ�¼
void displayTestRecords() {
    if (recordCount == 0) {
        printf(YELLOW "û�в��Լ�¼����ʾ��\n" RESET);
    } else {
        printf("\n" CYAN "===================== ���Լ�¼ =====================\n" RESET);
        for (int i = 0; i < recordCount; i++) {
            printf(MAGENTA "��¼ %d:\n" RESET, i + 1);
            printf(GREEN "����: %s\n" RESET, records[i].date);
            printf(BLUE "ģʽ: %s\n" RESET, records[i].mode);
            printf(YELLOW "��Ŀ����: %d\n" RESET, records[i].questions);
            printf(GREEN "�÷�: %d\n" RESET, records[i].score);
            printf("\n");
        }
        printf(CYAN "====================================================\n" RESET);
    }
}

// ���������ѧ��
void createQuestion(int *num1, int *num2, char *op) {
    int operation = rand() % 4;  // ���ѡ��4�ֲ������ӷ����������˷���������

    switch (operation) {
        case 0:  // �ӷ�
            *op = '+';
            *num1 = rand() % 101;
            *num2 = rand() % 101;
            break;
        case 1:  // ����
            *op = '-';
            *num1 = rand() % 101;
            *num2 = rand() % 101;
            break;
        case 2:  // �˷�
            *op = '*';
            *num1 = rand() % 11;  // �˷�����������Ϊ0-10֮��
            *num2 = rand() % 11;
            while (*num1 * *num2 > 100) {  // ȷ���˷����������100
                *num1 = rand() % 11;
                *num2 = rand() % 11;
            }
            break;
        case 3:  // ����
            *op = '/';
            *num1 = (rand() % 10 + 1) * (rand() % 10 + 1);  // ��֤num1��num2��������
            *num2 = rand() % 10 + 1;
            while (*num1 % *num2 != 0) {  // ȷ��������
                *num1 = (rand() % 10 + 1) * (rand() % 10 + 1);
                *num2 = rand() % 10 + 1;
            }
            break;
        default:
            *op = '+';  // Ĭ�ϼӷ�
            *num1 = rand() % 101;
            *num2 = rand() % 101;
            break;
    }
}

// �����
int solveQuestion(int num1, int num2, char op) {
    int result;
    switch (op) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/': result = num1 / num2; break;
        default: result = 0; break;
    }
    return result;
}

// �̶���Ŀ����ģʽ��30�⣬15���ӣ�
TestRecord fixedQuestionMode() {
    int correctAnswers = 0, totalQuestions = 30;
    time_t startTime = time(NULL);
    double timeLimit = 900; // 15����
    double remainingTime;

    for (int i = 0; i < totalQuestions; i++) {
        int num1, num2, userAnswer, correctAnswer;
        char op;

        createQuestion(&num1, &num2, &op);
        printf("\n" YELLOW "���� %d: %d %c %d = " RESET, i + 1, num1, op, num2);
        scanf("%d", &userAnswer);

        correctAnswer = solveQuestion(num1, num2, op);
        if (userAnswer == correctAnswer) correctAnswers++;

        double elapsedTime = difftime(time(NULL), startTime);
        remainingTime = timeLimit - elapsedTime;

        if (remainingTime < 30) {
            printf(RED "���棺ʣ��ʱ�䲻��30�롣\n" RESET);
        }

        if (elapsedTime > timeLimit) {
            printf(RED "ʱ�䵽��\n" RESET);
            break;
        }
    }

    double accuracy = ((double) correctAnswers / totalQuestions) * 100;
    double grade = accuracy + (remainingTime / 60.0); // �÷� = ��ȷ�� + ʣ��ʱ�䣨���ӣ�
    printf(GREEN "������ %d/%d ����Ŀ����ȷ��: %.2f%%\n�ɼ�Ϊ %.2f\n" RESET, correctAnswers, totalQuestions, accuracy, grade);

    TestRecord record;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(record.date, sizeof(record.date), "%Y-%m-%d", tm_info);
    strcpy(record.mode, "�̶���Ŀģʽ");
    record.questions = totalQuestions;
    record.score = (int) grade;
    return record;
}

// �̶�ʱ��ģʽ��3���ӣ�
TestRecord fixedTimeMode() {
    int correctAnswers = 0, totalQuestions = 0;
    time_t startTime = time(NULL);

    while (difftime(time(NULL), startTime) < 180) { // 3������
        int num1, num2, userAnswer, correctAnswer;
        char op;

        createQuestion(&num1, &num2, &op);
        printf("\n" YELLOW "���� %d: %d %c %d = " RESET, totalQuestions + 1, num1, op, num2);
        scanf("%d", &userAnswer);

        correctAnswer = solveQuestion(num1, num2, op);
        if (userAnswer == correctAnswer) correctAnswers++;

        totalQuestions++;
    }

    double accuracy = ((double) correctAnswers / totalQuestions) * 100;
    double grade = (double) accuracy * correctAnswers; // �÷� = ��ȷ�� * ��ȷ������
    printf(GREEN "������ %d/%d ����Ŀ����ȷ��: %.2f%%\n�ɼ�Ϊ %.2f\n" RESET, correctAnswers, totalQuestions, accuracy, grade);

    TestRecord record;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(record.date, sizeof(record.date), "%Y-%m-%d", tm_info);
    strcpy(record.mode, "�̶�ʱ��ģʽ");
    record.questions = totalQuestions;
    record.score = (int) grade;
    return record;
}

int main() {
    srand(time(NULL));
    int choice;

    printf(CYAN "��ӭʹ��С�ǵĿ������\n" RESET);
    printf("��ѡ��ģʽ��\n1. �̶���Ŀ����ģʽ��30�⣬15���ӣ�\n2. �̶�ʱ��ģʽ��3���ӣ�\n");
    printf("���������ѡ��: ");
    scanf("%d", &choice);

    TestRecord newRecord;
    switch (choice) {
        case 1:
            newRecord = fixedQuestionMode();
            break;
        case 2:
            newRecord = fixedTimeMode();
            break;
        default:
            printf(RED "��Ч��ѡ��\n" RESET);
            return 0;
    }

    // �����¼���ڴ���ļ�
    storeTestRecord(newRecord.date, newRecord.mode, newRecord.questions, newRecord.score);

    printf("====================================================\n");
    printf("����֤�������:\n");
    displayTestRecords();  // ��ӡ���Լ�¼

    return 0;
}
