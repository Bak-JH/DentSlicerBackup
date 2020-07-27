if [ "$#" -ne 2 ]
then
echo "usage: test.sh [local_product_path] [product_name]"
exit 1
fi

#need to edit pem path
pem_path="/mnt/c/Users/bakjh/Downloads/hixbananatest.pem"

ssh -i ${pem_path} \
    ubuntu@ec2-52-79-239-4.ap-northeast-2.compute.amazonaws.com \
    "mkdir -p ~/HixProductServer/SetupFiles/$2" \
&& \
scp -i ${pem_path} \
    -r $1 ubuntu@ec2-52-79-239-4.ap-northeast-2.compute.amazonaws.com:~/HixProductServer/SetupFiles/$2

