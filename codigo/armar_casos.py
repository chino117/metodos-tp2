import sys
import random as rand

# Como usar
# python3 armar_casos.py [tamaño][porcentaje de test]
# path output es la carpeta donde se van a escribir los archivos



def armar_archivo_csv(test_arreglo_neg, test_arreglo_pos, tipo, exp_size, test_size):
    path_test = tipo+"."+str(exp_size)+"."+str(test_size)+".csv"
    with open(path_test, mode='w+') as f:
        for i in test_arreglo_neg:
            f.write(str(i)+ ", "+ str(0) + ", \n" )
        for i in test_arreglo_pos:
            f.write(str(i)+ ", "+ str(1) + ", \n" )

if __name__ == "__main__":
    exp_size = int(sys.argv[1])
    test_perc = int(sys.argv[2])

    train_size = int(exp_size * (100-test_perc) / 100)
    test_size = exp_size-train_size

    test_neg_sample = rand.sample(range(0,12500), int(test_size/2) )
    test_pos_sample = rand.sample(range(12500,25000), test_size - int(test_size/2) )
    train_neg_sample = rand.sample(range(25000,37500), int(train_size/2) )
    train_pos_sample = rand.sample(range(37500,50000), train_size - int(train_size/2) )

    armar_archivo_csv(train_neg_sample,train_pos_sample, "train", exp_size, test_size)
    armar_archivo_csv(test_neg_sample,test_pos_sample, "test", exp_size, test_size)
