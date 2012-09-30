for i in *.u.txt; do
    iconv  -c --from-code=UTF-8 --to-code=ISO-8859-1 $i > $(echo $i| cut -d . -f 1)-8859.txt
done