#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Использование: %s <elf-файл> <смещение> <новый байт>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    unsigned long offset = strtoul(argv[2], NULL, 0); // Смещение в файле
    unsigned char new_byte = (unsigned char)strtoul(argv[3], NULL, 0); // Новый байт

    // Открываем файл для чтения и записи
    int fd = open(filename, O_RDWR);
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

    // Записываем новый байт
    if (write(fd, &new_byte, 1) != 1) {
        perror("Ошибка записи байта");
        close(fd);
        return 1;
    }

    printf("Байт по смещению 0x%lx успешно изменен на 0x%02x\n", offset, new_byte);

    close(fd);
    return 0;
}
