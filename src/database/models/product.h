//
// Created by fkkha on 31/08/2025.
//

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#ifndef INVENTORY_SYSTEM_PRODUCT_H
#define INVENTORY_SYSTEM_PRODUCT_H

struct Product {
    int id = 0;
    std::string name;
    std::string sku;
    std::string description;
    double price = 0.0;
    int quantity = 0;
    int min_quantity = 0;

    [[nodiscard]] nlohmann::json toJson() const;
    static Product fromJson(const nlohmann::json& j);

    static std::vector<Product> getAllProducts();

    static Product getProductById(int id);
    static Product getProductBySku(const std::string& sku);

    bool save();
    bool remove();
    [[nodiscard]] bool isValid() const;

};

#endif //INVENTORY_SYSTEM_PRODUCT_H
