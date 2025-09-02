//
// Created by fkkha on 31/08/2025.
//
#include "product.h"
#include "../connection.h"
#include <stdexcept>
#include <iostream>

Product Product::fromJson(const nlohmann::json &j) {
    Product product;

    if (j.contains("id")) product.id = j["id"];
    if (j.contains("sku")) product.sku = j["sku"];
    if (j.contains("name")) product.name = j["name"];
    if (j.contains("description")) product.description = j["description"];
    if (j.contains("price")) product.price = j["price"];
    if (j.contains("quantity")) product.quantity = j["quantity"];
    if (j.contains("min_quantity")) product.min_quantity = j["min_quantity"];

    return product;
}

nlohmann::json Product::toJson() const {
    return nlohmann::json{
            {"id", id},
            {"sku", sku},
            {"name", name},
            {"description", description},
            {"price", price},
            {"quantity", quantity},
            {"min_quantity", min_quantity}
    };
}

std::vector<Product> Product::getAllProducts() {
    std::vector<Product> products;
    sqlite3* db = Database::getConnection();
    sqlite3_stmt* stmt = nullptr;

    const char* sql = R"(
        SELECT id, sku, name, description, price, quantity, min_quantity
        FROM products
        ORDER BY name
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Product product;

            product.id = sqlite3_column_int(stmt, 0);

            const unsigned char* sku_text = sqlite3_column_text(stmt, 1);
            product.sku = sku_text ? (char*)sku_text : "";

            const unsigned char* name_text = sqlite3_column_text(stmt, 2);
            product.name = name_text ? (char*)name_text : "";

            const unsigned char* desc_text = sqlite3_column_text(stmt, 3);
            product.description = desc_text ? (char*)desc_text : "";

            product.price = sqlite3_column_double(stmt, 4);
            product.quantity = sqlite3_column_int(stmt, 5);
            product.min_quantity = sqlite3_column_int(stmt, 6);

            products.push_back(product);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "SQL error in getAllProducts: " << sqlite3_errmsg(db) << std::endl;
    }

    return products;
}

Product Product::getProductById(int id) {
    Product product;
    sqlite3* db = Database::getConnection();
    sqlite3_stmt* stmt = nullptr;

    const char* sql = R"(
        SELECT id, sku, name, description, price, quantity, min_quantity
        FROM products
        WHERE id = ?
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            product.id = sqlite3_column_int(stmt, 0);

            const unsigned char* sku_text = sqlite3_column_text(stmt, 1);
            product.sku = sku_text ? (char*)sku_text : "";

            const unsigned char* name_text = sqlite3_column_text(stmt, 2);
            product.name = name_text ? (char*)name_text : "";

            const unsigned char* desc_text = sqlite3_column_text(stmt, 3);
            product.description = desc_text ? (char*)desc_text : "";

            product.price = sqlite3_column_double(stmt, 4);
            product.quantity = sqlite3_column_int(stmt, 5);
            product.min_quantity = sqlite3_column_int(stmt, 6);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "SQL error in getProductById: " << sqlite3_errmsg(db) << std::endl;
    }

    return product;
}

Product Product::getProductBySku(const std::string& sku) {
    Product product; // Default constructor sets id = 0
    sqlite3* db = Database::getConnection();
    sqlite3_stmt* stmt = nullptr;

    const char* sql = R"(
        SELECT id, sku, name, description, price, quantity, min_quantity
        FROM products
        WHERE sku = ?
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, sku.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            product.id = sqlite3_column_int(stmt, 0);

            const unsigned char* sku_text = sqlite3_column_text(stmt, 1);
            product.sku = sku_text ? (char*)sku_text : "";

            const unsigned char* name_text = sqlite3_column_text(stmt, 2);
            product.name = name_text ? (char*)name_text : "";

            const unsigned char* desc_text = sqlite3_column_text(stmt, 3);
            product.description = desc_text ? (char*)desc_text : "";

            product.price = sqlite3_column_double(stmt, 4);
            product.quantity = sqlite3_column_int(stmt, 5);
            product.min_quantity = sqlite3_column_int(stmt, 6);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "SQL error in getProductBySku: " << sqlite3_errmsg(db) << std::endl;
    }

    return product;
}

// Instance methods
bool Product::save() {
    if (!isValid()) {
        return false;
    }

    sqlite3* db = Database::getConnection();
    sqlite3_stmt* stmt = nullptr;

    if (id == 0) {
        // INSERT new product
        const char* sql = R"(
            INSERT INTO products (sku, name, description, price, quantity, min_quantity)
            VALUES (?, ?, ?, ?, ?, ?)
        )";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, sku.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, description.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 4, price);
            sqlite3_bind_int(stmt, 5, quantity);
            sqlite3_bind_int(stmt, 6, min_quantity);

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                id = (int)sqlite3_last_insert_rowid(db);  // Get the new ID
                sqlite3_finalize(stmt);
                return true;
            } else {
                std::cerr << "INSERT error: " << sqlite3_errmsg(db) << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
        }

    } else {
        // UPDATE existing product
        const char* sql = R"(
            UPDATE products
            SET sku=?, name=?, description=?, price=?, quantity=?, min_quantity=?,
                updated_at=CURRENT_TIMESTAMP
            WHERE id=?
        )";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, sku.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, description.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 4, price);
            sqlite3_bind_int(stmt, 5, quantity);
            sqlite3_bind_int(stmt, 6, min_quantity);
            sqlite3_bind_int(stmt, 7, id);  // WHERE clause

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                sqlite3_finalize(stmt);
                return true;
            } else {
                std::cerr << "UPDATE error: " << sqlite3_errmsg(db) << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    return false;
}

bool Product::remove() {
    if (id == 0) {
        return false;  // Can't delete unsaved product
    }

    sqlite3* db = Database::getConnection();
    sqlite3_stmt* stmt = nullptr;

    const char* sql = "DELETE FROM products WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            id = 0;  // Reset ID since it's deleted
            return true;
        } else {
            std::cerr << "DELETE error: " << sqlite3_errmsg(db) << std::endl;
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
    }

    return false;
}

bool Product::isValid() const {
    // Basic validation rules
    if (sku.empty()) {
        std::cerr << "Validation error: SKU cannot be empty" << std::endl;
        return false;
    }

    if (name.empty()) {
        std::cerr << "Validation error: Name cannot be empty" << std::endl;
        return false;
    }

    if (price < 0.0) {
        std::cerr << "Validation error: Price cannot be negative" << std::endl;
        return false;
    }

    if (quantity < 0) {
        std::cerr << "Validation error: Quantity cannot be negative" << std::endl;
        return false;
    }

    if (min_quantity < 0) {
        std::cerr << "Validation error: Min quantity cannot be negative" << std::endl;
        return false;
    }

    if (sku.length() > 50) {
        std::cerr << "Validation error: SKU too long (max 50 characters)" << std::endl;
        return false;
    }

    if (name.length() > 50) {
        std::cerr << "Validation error: Name too long (max 50 characters)" << std::endl;
        return false;
    }

    return true;
}