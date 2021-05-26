/**
 * @author Muhammed Oğuz
 * 
 * This class Test all methods and provides a menu for testing CSE222_hw06 classes.
 */

import CSE222_hw06.src_oguz.*;
import java.util.*;

public class Test {
    public static void main(String[] args) {
        Company co = new Company();
        co.readFile("Data/e-commerce-samples.csv");


        Trader trader = new Trader("Alisha", "1234", true);
        ArrayList<String> tempArr = new ArrayList<String>();
        
        tempArr.add("Clothes");
        tempArr.add("Important Sub Category");
        Product tempPro = new Product("213", "product", "150", "150", "very important", "name");
        tempPro.setCategory(tempArr);
        trader.addProduct(tempPro);
        
        co.signUpTrader(trader);
        

        Trader temp = co.getcTraders().get(0);

        temp.setShowCategory(true);
        temp.setShowDescription(true);
        temp.printProducts();
    }
}
