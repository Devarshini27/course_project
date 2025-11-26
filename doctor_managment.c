#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DOCTORS 200
#define NAME_LEN 50
#define SPEC_LEN 50
#define PHONE_LEN 20
#define DATA_FILE "doctors.dat"

typedef struct {
    int id;
    char name[NAME_LEN];
    char specialty[SPEC_LEN];
    char phone[PHONE_LEN];
    int experience; 
} Doctor;

Doctor doctors[MAX_DOCTORS];
int doctor_count = 0;

void read_line(char *buffer, int size) {
    if (fgets(buffer, size, stdin)) {
        size_t ln = strlen(buffer);
        if (ln > 0 && buffer[ln-1] == '\n') buffer[ln-1] = '\0';
    } else {
        buffer[0] = '\0';
    }
}

void load_data() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) {
        doctor_count = 0;
        return;
    }
    size_t read = fread(&doctor_count, sizeof(int), 1, fp);
    if (read != 1) {
        doctor_count = 0;
        fclose(fp);
        return;
    }
    if (doctor_count > 0 && doctor_count <= MAX_DOCTORS) {
        fread(doctors, sizeof(Doctor), doctor_count, fp);
    } else {
        doctor_count = 0;
    }
    fclose(fp);
}

void save_data() {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp) {
        printf("Error: cannot open data file for writing.\n");
        return;
    }
    fwrite(&doctor_count, sizeof(int), 1, fp);
    if (doctor_count > 0) {
        fwrite(doctors, sizeof(Doctor), doctor_count, fp);
    }
    fclose(fp);
}

int find_index_by_id(int id) {
    for (int i = 0; i < doctor_count; ++i) {
        if (doctors[i].id == id) return i;
    }
    return -1;
}

int generate_new_id() {
    int maxid = 0;
    for (int i = 0; i < doctor_count; ++i)
        if (doctors[i].id > maxid) maxid = doctors[i].id;
    return maxid + 1;
}

void add_doctor() {
    if (doctor_count >= MAX_DOCTORS) {
        printf("Cannot add more doctors (limit reached).\n");
        return;
    }

    Doctor d;
    d.id = generate_new_id();

    printf("Enter doctor's name: ");
    read_line(d.name, NAME_LEN);

    printf("Enter specialty: ");
    read_line(d.specialty, SPEC_LEN);

    printf("Enter phone: ");
    read_line(d.phone, PHONE_LEN);

    char buf[32];
    int exp = 0;
    while (1) {
        printf("Enter years of experience (number): ");
        read_line(buf, sizeof(buf));
        if (sscanf(buf, "%d", &exp) == 1 && exp >= 0) break;
        printf("Invalid input. Please enter a non-negative integer.\n");
    }
    d.experience = exp;

    doctors[doctor_count++] = d;
    save_data();
    printf("Doctor added with ID %d.\n", d.id);
}

void list_doctors() {
    if (doctor_count == 0) {
        printf("No doctors in the system.\n");
        return;
    }
    printf("\nList of Doctors (%d):\n", doctor_count);
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < doctor_count; ++i) {
        Doctor *d = &doctors[i];
        printf("ID: %d\nName: %s\nSpecialty: %s\nPhone: %s\nExperience: %d years\n",
               d->id, d->name, d->specialty, d->phone, d->experience);
        printf("------------------------------------------------------------\n");
    }
}

void search_doctor() {
    char buf[32];
    int id;
    printf("Enter doctor ID to search: ");
    read_line(buf, sizeof(buf));
    if (sscanf(buf, "%d", &id) != 1) {
        printf("Invalid ID.\n");
        return;
    }
    int idx = find_index_by_id(id);
    if (idx == -1) {
        printf("Doctor with ID %d not found.\n", id);
        return;
    }
    Doctor *d = &doctors[idx];
    printf("Found doctor:\n");
    printf("ID: %d\nName: %s\nSpecialty: %s\nPhone: %s\nExperience: %d years\n",
           d->id, d->name, d->specialty, d->phone, d->experience);
}

void update_doctor() {
    char buf[32];
    int id;
    printf("Enter doctor ID to update: ");
    read_line(buf, sizeof(buf));
    if (sscanf(buf, "%d", &id) != 1) {
        printf("Invalid ID.\n");
        return;
    }
    int idx = find_index_by_id(id);
    if (idx == -1) {
        printf("Doctor with ID %d not found.\n", id);
        return;
    }
    Doctor *d = &doctors[idx];
    printf("Updating doctor (leave field blank to keep current value)\n");
    printf("Current name: %s\nNew name: ", d->name);
    char tmp[NAME_LEN];
    read_line(tmp, NAME_LEN);
    if (strlen(tmp) > 0) strncpy(d->name, tmp, NAME_LEN);

    printf("Current specialty: %s\nNew specialty: ", d->specialty);
    read_line(tmp, SPEC_LEN);
    if (strlen(tmp) > 0) strncpy(d->specialty, tmp, SPEC_LEN);

    printf("Current phone: %s\nNew phone: ", d->phone);
    read_line(tmp, PHONE_LEN);
    if (strlen(tmp) > 0) strncpy(d->phone, tmp, PHONE_LEN);

    printf("Current experience: %d\nNew experience (number): ", d->experience);
    read_line(tmp, sizeof(tmp));
    int newexp;
    if (strlen(tmp) > 0 && sscanf(tmp, "%d", &newexp) == 1 && newexp >= 0) {
        d->experience = newexp;
    } else if (strlen(tmp) > 0) {
        printf("Invalid experience input; keeping old value.\n");
    }

    save_data();
    printf("Doctor with ID %d updated.\n", d->id);
}

void delete_doctor() {
    char buf[32];
    int id;
    printf("Enter doctor ID to delete: ");
    read_line(buf, sizeof(buf));
    if (sscanf(buf, "%d", &id) != 1) {
        printf("Invalid ID.\n");
        return;
    }
    int idx = find_index_by_id(id);
    if (idx == -1) {
        printf("Doctor with ID %d not found.\n", id);
        return;
    }
    for (int i = idx; i < doctor_count - 1; ++i) {
        doctors[i] = doctors[i + 1];
    }
    doctor_count--;
    save_data();
    printf("Doctor with ID %d deleted.\n", id);
}

void show_menu() {
    printf("\n=== Doctor Management ===\n");
    printf("1. Add doctor\n");
    printf("2. View all doctors\n");
    printf("3. Search doctor by ID\n");
    printf("4. Update doctor by ID\n");
    printf("5. Delete doctor by ID\n");
    printf("6. Exit\n");
    printf("Choose an option (1-6): ");
}

int main() {
    load_data();
    while (1) {
        show_menu();
        char choice_buf[8];
        read_line(choice_buf, sizeof(choice_buf));
        int choice = 0;
        if (sscanf(choice_buf, "%d", &choice) != 1) {
            printf("Invalid choice. Please enter a number 1-6.\n");
            continue;
        }
        switch (choice) {
            case 1: add_doctor(); break;
            case 2: list_doctors(); break;
            case 3: search_doctor(); break;
            case 4: update_doctor(); break;
            case 5: delete_doctor(); break;
            case 6:
                printf("Exiting. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}
