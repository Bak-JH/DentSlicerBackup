if [ "$#" -ne 4 ]
then
echo "usage: upload.sh [setup_file_path] [product_name] [product_version] [patch_note_path]"
exit 1
fi
#need to edit pem path
pem_path=${HIXHOME_PEM}
echo $pem_path

LANG='POSIX'
date=$(date +'%a, %d %b %y')

echo $date

echo \
"<rss xmlns:sparkle='http://www.andymatuschak.org/xml-namespaces/sparkle' version='$3'>
    <channel>
        <title>$2</title>
        <description>Most recent updates to $2</description>
        <language>ko</language>
        <item>
            <title>Version $3</title>
            <sparkle:releaseNotesLink> https://services.hix.co.kr/setup/view_file/$2/patch_$3.html </sparkle:releaseNotesLink>
            <pubDate>$date</pubDate>
            <enclosure url='https://services.hix.co.kr/setup/get_file/dentslicer/DentSlicerSetup.exe' sparkle:version='$3' length='0' type='application/octet-stream'/>
        </item>
    </channel>
</rss>" > appcast.xml

ssh -i ${pem_path} \
    ubuntu@ec2-52-79-239-4.ap-northeast-2.compute.amazonaws.com \
    "mkdir -p ~/HixProductServer/SetupFiles/$2" \
&& \
scp -i ${pem_path} \
    -r $1 \
    ubuntu@ec2-52-79-239-4.ap-northeast-2.compute.amazonaws.com:~/HixProductServer/SetupFiles/$2

scp -i ${pem_path} \
    -r appcast.xml \
    ubuntu@ec2-52-79-239-4.ap-northeast-2.compute.amazonaws.com:~/HixProductServer/SetupFiles/$2

scp -i ${pem_path} \
    -r $4 \
    ubuntu@ec2-52-79-239-4.ap-northeast-2.compute.amazonaws.com:~/HixProductServer/SetupFiles/$2/patch_$3.html

# this part for local test #

# mkdir -p ./$2
# scp -r $1 ./$2
# scp -r appcast.xml ./$2
# scp -r $4 ./$2/patch_$3.html