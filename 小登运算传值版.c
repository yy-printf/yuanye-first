#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 用于控制彩色输出
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// 结构体定义
typedef struct {
    char date[20];
    char mode[20];
    int questions;
    int score;
} TestRecord;

#define MAX_RECORDS 100

TestRecord records[MAX_RECORDS];
int recordCount = 0;

// 将记录写入文件
void saveRecordToFile(TestRecord* record) {
    FILE *file = fopen("test_records.txt", "a"); // 打开文件（追加模式）
    if (file == NULL) {
        printf(RED "无法打开文件！\n" RESET);
        return;
    }

    // 写入记录到文件
    fprintf(file, "Date: %s\n", record->date);
    fprintf(file, "Mode: %s\n", record->mode);
    fprintf(file, "Questions: %d\n", record->questions);
    fprintf(file, "Score: %d\n", record->score);
    fprintf(file, "\n");

    // 关闭文件
    fclose(file);
}

// 将测试记录添加到内存数组并写入文件
void storeTestRecord(const char* date, const char* mode, int questions, int score) {
    if (recordCount < MAX_RECORDS) {
        strcpy(records[recordCount].date, date);
        strcpy(records[recordCount].mode, mode);
        records[recordCount].questions = questions;
        records[recordCount].score = score;
        recordCount++;

        // 将记录写入文件
        saveRecordToFile(&records[recordCount - 1]);
    } else {
        printf(RED "无法添加更多记录，数组已满。\n" RESET);
    }
}

// 显示所有测试记录
void displayTestRecords() {
    if (recordCount == 0) {
        printf(YELLOW "没有测试记录可显示！\n" RESET);
    } else {
        printf("\n" CYAN "===================== 测试记录 =====================\n" RESET);
        for (int i = 0; i < recordCount; i++) {
            printf(MAGENTA "记录 %d:\n" RESET, i + 1);
            printf(GREEN "日期: %s\n" RESET, records[i].date);
            printf(BLUE "模式: %s\n" RESET, records[i].mode);
            printf(YELLOW "题目数量: %d\n" RESET, records[i].questions);
            printf(GREEN "得分: %d\n" RESET, records[i].score);
            printf("\n");
        }
        printf(CYAN "====================================================\n" RESET);
    }
}

// 生成随机数学题
void createQuestion(int *num1, int *num2, char *op) {
    int operation = rand() % 4;  // 随机选择4种操作（加法、减法、乘法、除法）

    switch (operation) {
        case 0:  // 加法
            *op = '+';
            *num1 = rand() % 101;
            *num2 = rand() % 101;
            break;
        case 1:  // 减法
            *op = '-';
            *num1 = rand() % 101;
            *num2 = rand() % 101;
            break;
        case 2:  // 乘法
            *op = '*';
            *num1 = rand() % 11;  // 乘法的数字限制为0-10之间
            *num2 = rand() % 11;
            while (*num1 * *num2 > 100) {  // 确保乘法结果不超过100
                *num1 = rand() % 11;
                *num2 = rand() % 11;
            }
            break;
        case 3:  // 除法
            *op = '/';
            *num1 = (rand() % 10 + 1) * (rand() % 10 + 1);  // 保证num1是num2的整数倍
            *num2 = rand() % 10 + 1;
            while (*num1 % *num2 != 0) {  // 确保能整除
                *num1 = (rand() % 10 + 1) * (rand() % 10 + 1);
                *num2 = rand() % 10 + 1;
            }
            break;
        default:
            *op = '+';  // 默认加法
            *num1 = rand() % 101;
            *num2 = rand() % 101;
            break;
    }
}

// 计算答案
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

// 固定题目数量模式（30题，15分钟）
TestRecord fixedQuestionMode() {
    int correctAnswers = 0, totalQuestions = 30;
    time_t startTime = time(NULL);
    double timeLimit = 900; // 15分钟
    double remainingTime;

    for (int i = 0; i < totalQuestions; i++) {
        int num1, num2, userAnswer, correctAnswer;
        char op;

        createQuestion(&num1, &num2, &op);
        printf("\n" YELLOW "问题 %d: %d %c %d = " RESET, i + 1, num1, op, num2);
        scanf("%d", &userAnswer);

        correctAnswer = solveQuestion(num1, num2, op);
        if (userAnswer == correctAnswer) correctAnswers++;

        double elapsedTime = difftime(time(NULL), startTime);
        remainingTime = timeLimit - elapsedTime;

        if (remainingTime < 30) {
            printf(RED "警告：剩余时间不足30秒。\n" RESET);
        }

        if (elapsedTime > timeLimit) {
            printf(RED "时间到！\n" RESET);
            break;
        }
    }

    double accuracy = ((double) correctAnswers / totalQuestions) * 100;
    double grade = accuracy + (remainingTime / 60.0); // 得分 = 正确率 + 剩余时间（分钟）
    printf(GREEN "你答对了 %d/%d 道题目。正确率: %.2f%%\n成绩为 %.2f\n" RESET, correctAnswers, totalQuestions, accuracy, grade);

    TestRecord record;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(record.date, sizeof(record.date), "%Y-%m-%d", tm_info);
    strcpy(record.mode, "固定题目模式");
    record.questions = totalQuestions;
    record.score = (int) grade;
    return record;
}

// 固定时间模式（3分钟）
TestRecord fixedTimeMode() {
    int correctAnswers = 0, totalQuestions = 0;
    time_t startTime = time(NULL);

    while (difftime(time(NULL), startTime) < 180) { // 3分钟内
        int num1, num2, userAnswer, correctAnswer;
        char op;

        createQuestion(&num1, &num2, &op);
        printf("\n" YELLOW "问题 %d: %d %c %d = " RESET, totalQuestions + 1, num1, op, num2);
        scanf("%d", &userAnswer);

        correctAnswer = solveQuestion(num1, num2, op);
        if (userAnswer == correctAnswer) correctAnswers++;

        totalQuestions++;
    }

    double accuracy = ((double) correctAnswers / totalQuestions) * 100;
    double grade = (double) accuracy * correctAnswers; // 得分 = 正确率 * 正确答案数量
    printf(GREEN "你答对了 %d/%d 道题目。正确率: %.2f%%\n成绩为 %.2f\n" RESET, correctAnswers, totalQuestions, accuracy, grade);

    TestRecord record;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(record.date, sizeof(record.date), "%Y-%m-%d", tm_info);
    strcpy(record.mode, "固定时间模式");
    record.questions = totalQuestions;
    record.score = (int) grade;
    return record;
}

int main() {
    srand(time(NULL));
    int choice;

    printf(CYAN "欢迎使用小登的口算程序！\n" RESET);
    printf("请选择模式：\n1. 固定题目数量模式（30题，15分钟）\n2. 固定时间模式（3分钟）\n");
    printf("请输入你的选择: ");
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
            printf(RED "无效的选择！\n" RESET);
            return 0;
    }

    // 保存记录到内存和文件
    storeTestRecord(newRecord.date, newRecord.mode, newRecord.questions, newRecord.score);

    printf("====================================================\n");
    printf("请验证你的输入:\n");
    displayTestRecords();  // 打印测试记录

    return 0;
}
