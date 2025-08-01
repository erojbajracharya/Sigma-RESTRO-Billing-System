#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_ITEMS 15
#define MAX_INVOICE_ITEMS 20
#define MENU_FILE "menu.dat"
#define INVOICES_FILE "invoices.dat"

struct MenuItem
{
    int id;
    char description[50];
    float price;
};

struct InvoiceItem
{
    int item_id;
    char description[50];
    int quantity;
    float price;
    float total;
};

struct Invoice
{
    int customer_id;
    char transaction_date[11];
    int table_no;
    struct InvoiceItem items[MAX_INVOICE_ITEMS];
    int item_count;
    float subtotal;
    float vat;
    float discount;
    float net_amount;
};

void initializeMenu();
void displayMainMenu();
void createInvoice();
void searchInvoice();
void exitProgram();
void addItem(struct Invoice *invoice);
void removeItem(struct Invoice *invoice);
void applyDiscount(struct Invoice *invoice);
void finalizeInvoice(struct Invoice *invoice);
void printInvoice(struct Invoice invoice);
void cancelInvoice(struct Invoice *invoice);
int isCustomerIdUnique(int customer_id);
int validateDate(const char *date);
void displayMenuItems();
struct MenuItem *findMenuItem(int item_id);

struct MenuItem menuItems[MAX_ITEMS] = {
    {101, "Momo Platter", 350.00},
    {102, "Sekuwa Skewers", 400.00},
    {103, "Aloo Tama", 250.00},
    {104, "Chatamari Bites", 300.00},
    {105, "Masala Papad", 150.00},
    {106, "Dal Bhat Set", 450.00},
    {107, "Sekuwa Burger", 400.00},
    {108, "Thukpa Noodle", 350.00},
    {109, "Chicken Curry", 400.00},
    {110, "Vegetable Curry", 350.00},
    {111, "Gundruk Achar", 200.00},
    {112, "Bhatmas Sadeko", 180.00},
    {113, "Butter Naan", 220.00},
    {114, "Gulab Jamun", 200.00},
    {115, "Sel Roti", 150.00}};

int main()
{
    initializeMenu();

    int choice;
    do
    {
        displayMainMenu();
        printf("Enter Your Choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createInvoice();
            break;
        case 2:
            searchInvoice();
            break;
        case 3:
            exitProgram();
            break;
        default:
            printf("Invalid Choice! Please Try Again.\n");
        }
    } while (choice != 3);

    return 0;
}

void initializeMenu()
{
    FILE *file = fopen(MENU_FILE, "wb");
    if (file == NULL)
    {
        printf("Error Creating Menu File!\n");
        return;
    }
    fwrite(menuItems, sizeof(struct MenuItem), MAX_ITEMS, file);
    fclose(file);
}

void displayMainMenu()
{
    printf("\n=============================================\n");
    printf("          SIGMA RESTRO BILLING SYSTEM\n");
    printf("=============================================\n");
    printf("1. Create Invoice\n");
    printf("2. Search Invoice By Customer ID\n");
    printf("3. Exit Program\n");
    printf("=============================================\n");
}

void displayMenuItems()
{
    printf("\n==============================================\n");
    printf("                   MENU\n");
    printf("==============================================\n");
    printf("| Item ID | Description             | Price   |\n");
    printf("|---------|-------------------------|-------- |\n");

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        printf("| %-7d | %-23s | %7.2f |\n",
               menuItems[i].id, menuItems[i].description, menuItems[i].price);
    }
    printf("==============================================\n");
}

struct MenuItem *findMenuItem(int item_id)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (menuItems[i].id == item_id)
        {
            return &menuItems[i];
        }
    }
    return NULL;
}

void createInvoice()
{
    struct Invoice new_invoice;
    new_invoice.item_count = 0;
    new_invoice.subtotal = 0;
    new_invoice.vat = 0;
    new_invoice.discount = 0;
    new_invoice.net_amount = 0;

    printf("\nEnter Customer ID: ");
    scanf("%d", &new_invoice.customer_id);

    while (!isCustomerIdUnique(new_invoice.customer_id))
    {
        printf("Customer ID Already Exists. Enter A Unique ID: ");
        scanf("%d", &new_invoice.customer_id);
    }

    printf("Enter Transaction Date (YYYY-MM-DD): ");
    scanf("%10s", new_invoice.transaction_date);
    while (!validateDate(new_invoice.transaction_date))
    {
        printf("Invalid Date Format. Please Enter As YYYY-MM-DD: ");
        scanf("%10s", new_invoice.transaction_date);
    }

    printf("Enter Table Number: ");
    scanf("%d", &new_invoice.table_no);

    int choice;
    do
    {
        printf("\n1. Add Item\n");
        printf("2. Remove Item\n");
        printf("3. Apply Discount\n");
        printf("4. Finalize Invoice\n");
        printf("5. Cancel Invoice\n");
        printf("Enter Your Choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addItem(&new_invoice);
            break;
        case 2:
            removeItem(&new_invoice);
            break;
        case 3:
            applyDiscount(&new_invoice);
            break;
        case 4:
            finalizeInvoice(&new_invoice);
            break;
        case 5:
            cancelInvoice(&new_invoice);
            return;
        default:
            printf("Invalid Choice!\n");
        }
    } while (choice != 4);
}

void addItem(struct Invoice *invoice)
{
    displayMenuItems();

    int item_id, quantity;
    printf("\nEnter Item ID To Add: ");
    scanf("%d", &item_id);

    struct MenuItem *item = findMenuItem(item_id);
    if (item == NULL)
    {
        printf("Invalid Item ID! Please Try Again.\n");
        return;
    }

    printf("Enter Quantity: ");
    scanf("%d", &quantity);

    if (quantity <= 0)
    {
        printf("Quantity Must Be Positive!\n");
        return;
    }

    for (int i = 0; i < invoice->item_count; i++)
    {
        if (invoice->items[i].item_id == item_id)
        {
            invoice->items[i].quantity += quantity;
            invoice->items[i].total = invoice->items[i].quantity * invoice->items[i].price;
            invoice->subtotal += quantity * item->price;
            printf("Item Quantity Updated Successfully!\n");
            return;
        }
    }

    if (invoice->item_count >= MAX_INVOICE_ITEMS)
    {
        printf("Maximum Items Reached For This Invoice!\n");
        return;
    }

    struct InvoiceItem new_item;
    new_item.item_id = item_id;
    strcpy(new_item.description, item->description);
    new_item.quantity = quantity;
    new_item.price = item->price;
    new_item.total = quantity * item->price;

    invoice->items[invoice->item_count++] = new_item;
    invoice->subtotal += new_item.total;

    printf("Item Added Successfully!\n");
}

void removeItem(struct Invoice *invoice)
{
    if (invoice->item_count == 0)
    {
        printf("No Items To Remove!\n");
        return;
    }

    printf("\nCurrent Invoice Items:\n");
    printf("-------------------------------------------------------------\n");
    printf("Item ID   Description            QTY     Price     Total\n");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < invoice->item_count; i++)
    {
        printf("%-8d  %-23s %-5d   %-8.2f  %-8.2f\n",
               invoice->items[i].item_id,
               invoice->items[i].description,
               invoice->items[i].quantity,
               invoice->items[i].price,
               invoice->items[i].total);
    }
    printf("-------------------------------------------------------------\n");

    int item_id;
    printf("Enter Item ID To Remove: ");
    scanf("%d", &item_id);

    int found = 0;
    for (int i = 0; i < invoice->item_count; i++)
    {
        if (invoice->items[i].item_id == item_id)
        {
            invoice->subtotal -= invoice->items[i].total;

            for (int j = i; j < invoice->item_count - 1; j++)
            {
                invoice->items[j] = invoice->items[j + 1];
            }

            invoice->item_count--;
            found = 1;
            printf("Item Removed Successfully!\n");
            break;
        }
    }

    if (!found)
    {
        printf("Item Not Found In Invoice!\n");
    }
}

void applyDiscount(struct Invoice *invoice)
{
    if (invoice->item_count == 0)
    {
        printf("No Items In Invoice To Apply Discount!\n");
        return;
    }

    float discount_percent;
    printf("Enter Discount Percentage (0-100): ");
    scanf("%f", &discount_percent);

    if (discount_percent < 0 || discount_percent > 100)
    {
        printf("Invalid Discount Percentage! Must Be Between 0 And 100.\n");
        return;
    }

    invoice->discount = invoice->subtotal * (discount_percent / 100);
    printf("Discount Of %.2f%% Applied Successfully!\n", discount_percent);
}

void finalizeInvoice(struct Invoice *invoice)
{
    if (invoice->item_count == 0)
    {
        printf("Cannot Finalize Empty Invoice!\n");
        return;
    }

    invoice->vat = invoice->subtotal * 0.13;
    invoice->net_amount = invoice->subtotal + invoice->vat - invoice->discount;

    FILE *file = fopen(INVOICES_FILE, "ab");
    if (file == NULL)
    {
        printf("Error Saving Invoice!\n");
        return;
    }

    fwrite(invoice, sizeof(struct Invoice), 1, file);
    fclose(file);

    printf("\nInvoice Finalized And Saved Successfully!\n");
    printInvoice(*invoice);
}

void printInvoice(struct Invoice invoice)
{
    printf("\n-------------------------------------------------------------\n");
    printf("                 SIGMA RESTRO\n");
    printf("-------------------------------------------------------------\n");
    printf("Customer ID:        %d\n", invoice.customer_id);
    printf("Transaction Date:   %s\n", invoice.transaction_date);
    printf("Table No:           %d\n", invoice.table_no);
    printf("-------------------------------------------------------------\n");
    printf("Item ID   Description            QTY     Price     Total\n");
    printf("-------------------------------------------------------------\n");

    for (int i = 0; i < invoice.item_count; i++)
    {
        printf("%-8d  %-23s %-5d   %-8.2f  %-8.2f\n",
               invoice.items[i].item_id,
               invoice.items[i].description,
               invoice.items[i].quantity,
               invoice.items[i].price,
               invoice.items[i].total);
    }

    printf("-------------------------------------------------------------\n");
    printf("Sub Total :                                        %8.2f\n", invoice.subtotal);
    printf("VAT @13%%  :                                        %8.2f\n", invoice.vat);
    printf("Discount @%.0f%% :                                    %8.2f\n",
           (invoice.discount / invoice.subtotal * 100), invoice.discount);
    printf("-------------------------------------------------------------\n");
    printf("Net Amount :                                       %8.2f\n", invoice.net_amount);
    printf("-------------------------------------------------------------\n");
}

void cancelInvoice(struct Invoice *invoice)
{
    printf("Invoice Cancelled. No Changes Saved.\n");
}

void searchInvoice()
{
    int customer_id;
    printf("\nEnter Customer ID To Search: ");
    scanf("%d", &customer_id);
    
    FILE *file = fopen(INVOICES_FILE, "rb");
    if (file == NULL)
    {
        printf("No Invoices Found!\n");
        return;
    }

    struct Invoice invoice;
    int found = 0;

    printf("\nSearching For Invoices For Customer ID: %d\n", customer_id);

    while (fread(&invoice, sizeof(struct Invoice), 1, file))
    {
        if (invoice.customer_id == customer_id)
        {
            printInvoice(invoice);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No Invoices Found For Customer ID %d\n", customer_id);
    }

    fclose(file);
}

int isCustomerIdUnique(int customer_id)
{
    FILE *file = fopen(INVOICES_FILE, "rb");
    if (file == NULL)
        return 1;

    struct Invoice invoice;
    while (fread(&invoice, sizeof(struct Invoice), 1, file))
    {
        if (invoice.customer_id == customer_id)
        {
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

int validateDate(const char *date)
{
    if (strlen(date) != 10)
        return 0;
    if (date[4] != '-' || date[7] != '-')
        return 0;

    for (int i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
            continue;
        if (!isdigit(date[i]))
            return 0;
    }

    int year, month, day;
    sscanf(date, "%d-%d-%d", &year, &month, &day);

    if (year < 1900 || year > 2100)
        return 0;
    if (month < 1 || month > 12)
        return 0;
    if (day < 1 || day > 31)
        return 0;

    return 1;
}

void exitProgram()
{
    printf("\nThank You For Using SIGMA RESTRO BILLING SYSTEM!\n");
    printf("Exiting Program...\n");
    exit(0);
}