#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

// Функция для нормализации строки смещения
char* normalize_offset_string(const char *offset_str) {
    char *result;
    
    // Проверяем, начинается ли уже с 0x или 0X
    if (strlen(offset_str) >= 2 && offset_str[0] == '0' && (offset_str[1] == 'x' || offset_str[1] == 'X')) {
        result = malloc(strlen(offset_str) + 1);
        strcpy(result, offset_str);
        return result;
    }
    
    // Проверяем, состоит ли строка только из hex-символов
    int is_hex = 1;
    for (int i = 0; offset_str[i]; i++) {
        if (!isxdigit(offset_str[i])) {
            is_hex = 0;
            break;
        }
    }
    
    if (is_hex) {
        // Добавляем префикс 0x
        result = malloc(strlen(offset_str) + 3);
        sprintf(result, "0x%s", offset_str);
        return result;
    }
    
    // Если не похоже на hex, возвращаем как есть (возможно десятичное)
    result = malloc(strlen(offset_str) + 1);
    strcpy(result, offset_str);
    return result;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Использование:\n");
        fprintf(stderr, "  %s <elf-файл> <смещение> [новый байт] - просмотр/изменение\n", argv[0]);
        fprintf(stderr, "  %s -l <elf-файл> <смещение> - только чтение\n", argv[0]);
        return 1;
    }

    int read_only = 0;
    const char *filename;
    char *offset_str;
    unsigned long offset;
    unsigned char new_byte = 0;
    
    // Проверяем флаг -l
    if (strcmp(argv[1], "-l") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Использование: %s -l <elf-файл> <смещение>\n", argv[0]);
            return 1;
        }
        read_only = 1;
        filename = argv[2];
        offset_str = argv[3];
    } else {
        if (argc != 4 && argc != 3) {
            fprintf(stderr, "Использование: %s <elf-файл> <смещение> [новый байт]\n", argv[0]);
            return 1;
        }
        filename = argv[1];
        offset_str = argv[2];
        if (argc == 4) {
            new_byte = (unsigned char)strtoul(argv[3], NULL, 0);
        } else {
            read_only = 1; // Только просмотр
        }
    }
    
    // Нормализуем строку смещения (добавляем 0x если нужно)
    char *normalized_offset = normalize_offset_string(offset_str);
    offset = strtoul(normalized_offset, NULL, 0);
    
    printf("Смещение: %s -> 0x%lx\n", normalized_offset, offset);
    free(normalized_offset);

    // Открываем файл (для чтения или чтения/записи)
    int fd;
    if (read_only) {
        fd = open(filename, O_RDONLY);
    } else {
        fd = open(filename, O_RDWR);
    }
    
    if (fd < 0) {
        perror("Ошибка открытия файла");
        return 1;
    }

    // Переходим к указанному смещению
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("Ошибка позиционирования в файле");
        close(fd);
        return 1;
    }
    
    // Читаем байт
    unsigned char current_byte;
    if (read(fd, &current_byte, 1) != 1) {
        perror("Ошибка чтения байта");
        close(fd);
        return 1;
    }
    
    printf("Байт по смещению 0x%lx = 0x%02x \n", offset, current_byte);
    
    // Если не read-only и указан новый байт, то записываем
    if (!read_only && argc == 4) {
        // Возвращаемся к смещению для записи
        if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
            perror("Ошибка позиционирования в файле для записи");
            close(fd);
            return 1;
        }
        
        // Записываем новый байт
        if (write(fd, &new_byte, 1) != 1) {
            perror("Ошибка записи байта");
            close(fd);
            return 1;
        }
        
        printf("Байт по смещению 0x%lx успешно изменен с 0x%02x на %02x\n", 
               offset, current_byte, new_byte);
    }

    close(fd);
    return 0;
}
