/*
*File : RandomAccessFile.cpp
*Project: Assignment_1_Relational_Databases
*Programmer: Juan Jose Bejarano
*First version: 27/09/2024
*Description:
*This program manages an inventory system that allows users to add, view, update,
*delete, and load products stored in a binary file using random access. The data is
*stored in a structure that contains product details including ID, name, category,
*quantity, and price.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable: 4996) // required by Visual Studio

#define MAX_RECORDS 100
#define RECORD_SIZE 256
#define FILENAME "products.dat"

typedef struct {
    int productId;
    char name[50];
    char category[50];
    int quantity;
    float price;
} ProductRandomAccess;


//Function prototypes
void addProduct(int productId, ProductRandomAccess product);
ProductRandomAccess searchProductByPosition(int productId);
void updateProduct(int productId, ProductRandomAccess product);
void deleteProduct(int productId);
int calculateHash(int productId);
void loadProducts();

void menuRandomAccess();

/*
* FUNCTION : menuRandomAccess
* DESCRIPTION :
* This function displays the inventory management menu and handles user input
* for different operations like adding, viewing, updating, and deleting products.
* It runs in a loop until the user decides to exit.
*/

void menuRandomAccess()
{
    int choice, productId;
    ProductRandomAccess product;

    while (1) {
        printf("\nInventory Management System\n");
        printf("1. Add Product\n");
        printf("2. View Product by ID\n");
        printf("3. Update Product\n");
        printf("4. Delete Product\n");
        printf("5. Load All Products\n");
        printf("6. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
        case 1:
            printf("Enter Product ID: ");
            scanf("%d", &product.productId);
            getchar();
            printf("Enter Name: ");
            fgets(product.name, sizeof(product.name), stdin);
            product.name[strcspn(product.name, "\n")] = 0; // Remove newline
            printf("Enter Category: ");
            fgets(product.category, sizeof(product.category), stdin);
            product.category[strcspn(product.category, "\n")] = 0;
            printf("Enter Quantity: ");
            scanf("%d", &product.quantity);
            printf("Enter Price: ");
            scanf("%f", &product.price);
            addProduct(product.productId, product);
            break;
        case 2:
            printf("Enter Product ID to search: ");
            scanf("%d", &productId);
            product = searchProductByPosition(productId);
            printf("\nProduct ID: %d\nName: %s\nCategory: %s\nQuantity: %d\nPrice: %.2f\n",
                product.productId, product.name, product.category, product.quantity, product.price);
            break;
        case 3:
            printf("Enter Product ID to update: ");
            scanf("%d", &productId);
            printf("Enter new Name: ");
            getchar();
            fgets(product.name, sizeof(product.name), stdin);
            product.name[strcspn(product.name, "\n")] = 0;
            printf("Enter new Category: ");
            fgets(product.category, sizeof(product.category), stdin);
            product.category[strcspn(product.category, "\n")] = 0;
            printf("Enter new Quantity: ");
            scanf("%d", &product.quantity);
            printf("Enter new Price: ");
            scanf("%f", &product.price);
            updateProduct(productId, product);
            break;
        case 4:
            printf("Enter Product ID to delete: ");
            scanf("%d", &productId);
            deleteProduct(productId);
            break;
        case 5:
            loadProducts();
            break;
        case 6:
            char exitConfirmation;
            printf("Are you sure you want to exit? (y/n): ");
            scanf(" %c", &exitConfirmation);
            if (exitConfirmation == 'y' || exitConfirmation == 'Y') {
                exit(0);
            }
            break;

        default:
            printf("\nInvalid option. Please try again.\n");
        }

    }
}

/*
* FUNCTION : addProduct
* DESCRIPTION :
* This function adds a new product to the inventory by writing to the binary file.
* It checks for duplicate product IDs and handles file operations.
* PARAMETERS :
* int productId : The ID of the product to be added.
* ProductRandomAccess product : The product structure containing product details.
*/

void addProduct(int productId, ProductRandomAccess product) {
    FILE* file = fopen(FILENAME, "rb+");
    // change postion for product Id check if the product Id is 0 and check if there is another one with the same product id.
   // (if there is nothing with that product then create the product, else return error)
    if (productId == 0) {
        printf("You cant add a product with product Id = 0\n");
        exit(EXIT_FAILURE);
    }

    if (file == NULL) {
        printf("Cannot open file");
        exit(EXIT_FAILURE);
    }
    ProductRandomAccess existingProduct = searchProductByPosition(productId);
    if (existingProduct.productId != -1) {
        printf("Product ID %d already exists.\n", productId);
        fclose(file);
        return;
    }
    int whereToWriteTheRecord = calculateHash(productId);
    if (fseek(file, whereToWriteTheRecord, SEEK_SET) != 0) {
        printf("Error seeking to the position %d. \n", whereToWriteTheRecord);
        exit(EXIT_FAILURE);
    }
    if (fwrite(&product, sizeof(ProductRandomAccess), 1, file) != 1) {
        printf("Error writing to the file\n");
        exit(EXIT_FAILURE);
    }
    if (fclose(file) != 0) {
        printf("Error closing the file\n");
        exit(EXIT_FAILURE);
    }
    printf("Product added successfully.\n");
}

/*
* FUNCTION : searchProductByPosition
* DESCRIPTION :
* This function searches for a product in the binary file by its ID.
* PARAMETERS :
* int productId : The ID of the product to search for.
* RETURNS :
* ProductRandomAccess : The product structure containing the found product details,
* or a product with ID -1 if not found.
*/

ProductRandomAccess searchProductByPosition(int productId)
{
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Cannot open file");
        exit(EXIT_FAILURE);
    }
    int position = calculateHash(productId);
    if (fseek(file, position, SEEK_SET) != 0) {
        printf("Error seeking to the position %d. \n", productId);
        exit(EXIT_FAILURE);
    }
    ProductRandomAccess product;
    if (fread(&product, sizeof(ProductRandomAccess), 1, file) == 1 && product.productId == productId) {
        if (fclose(file) != 0) {
            printf("Error closing the file\n");
            exit(EXIT_FAILURE);
        }
        return product;
    }//error cheking
    if (fclose(file) != 0) {
        printf("Error closing the file\n");
        exit(EXIT_FAILURE);
    }
    ProductRandomAccess notFound = { -1,"","", 0, 0.0 };
    return notFound;
}

/*
* FUNCTION : loadProducts
* DESCRIPTION :
* This function loads all products from the binary file and displays them.
*/

void loadProducts() {
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Cannot open file \n");
        return;
    }

    ProductRandomAccess product;
    printf("\nLoaded Products: \n");
    while (fread(&product, sizeof(ProductRandomAccess), 1, file) == 1) {
        if (product.productId != -1) {  // Skip deleted products 
            printf("ID: %d, Name: %s, Category: %s, Quantity: %d, Price: %.2f\n",
                product.productId, product.name, product.category, product.quantity, product.price); 
        }
    }

    if (fclose(file) != 0) { 
        printf("Error closing the file\n"); 
        exit(EXIT_FAILURE); 
    }

}

/*
* FUNCTION : updateProduct
* DESCRIPTION :
* This function updates an existing product in the binary file.
* PARAMETERS :
* int position : The product ID to update.
* ProductRandomAccess product : The updated product structure.
*/

void updateProduct(int position, ProductRandomAccess product) {
    FILE* file = fopen(FILENAME, "rb+");
    if (file == NULL) {
        printf("Cannot open file");
        exit(EXIT_FAILURE);
    }
    int whereToReadTheRecord = calculateHash(position);
    if(fseek(file, whereToReadTheRecord, SEEK_SET) != 0) {
        printf("Error seeking to the position %d. \n", whereToReadTheRecord);
        exit(EXIT_FAILURE);
    }
   if (fwrite(&product, sizeof(ProductRandomAccess), 1, file) != 1) {
       printf("Error writing to the file\n");
       exit(EXIT_FAILURE);
    }
    if (fclose(file) != 0) {
        printf("Error closing the file\n");
        exit(EXIT_FAILURE);
    }
}

/*
* FUNCTION : deleteProduct
* DESCRIPTION :
* This function deletes a product from the binary file by marking it as deleted.
* PARAMETERS :
* int productId : The ID of the product to be deleted.
*/

void deleteProduct(int productId) {
    FILE* file = fopen(FILENAME, "rb+");
    if (file == NULL) {
        printf("Cannot open file");
        exit(EXIT_FAILURE);
    }
    int position = calculateHash(productId);
   if (fseek(file, position, SEEK_SET) != 0) {
       printf("Error seeking to the position \n");
       exit(EXIT_FAILURE);
   }

   ProductRandomAccess deletedProduct = { -1, "", "", 0, 0.0 };  // Mark as deleted
    if (fwrite(&deletedProduct, sizeof(ProductRandomAccess), 1, file) != 1) {
        printf("Error writing to the file\n");
        exit(EXIT_FAILURE);
    }

    if (fclose(file) != 0) {
        printf("Error closing the file\n");
        exit(EXIT_FAILURE);
    }
    printf("Product at productId %d has been deleted.\n", productId);
}

/*
* FUNCTION : calculateHash
* DESCRIPTION :
* This function calculates the hash position in the file for a given product ID.
* PARAMETERS :
* int productId : The ID of the product.
* RETURNS :
* int : The calculated position in the file for the product.
*/

int calculateHash(int productId) {
    return productId * sizeof(ProductRandomAccess);
}