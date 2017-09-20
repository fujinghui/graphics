#ifndef _LOGIN_H_
#define _LOGIN_H_
#include <string.h>
class FUserInfo{
public:
	int user_id;
	char user_name[64];
	char user_password[64];
	//�û���¼�ɹ�֮��
	int house_id = -1;
	SOCKET socket = NULL;
	FUserInfo* operator=(const FUserInfo &s){
		FUserInfo *sn = new FUserInfo;
		sn->user_id = s.user_id; 
		fstrcpy(sn->user_name, (char*)(&s.user_name), sizeof(s.user_name)); fstrcpy(sn->user_password, (char*)(&s.user_password), sizeof(s.user_password));
		return sn;
	}

	bool operator==(const FUserInfo &f1) const{
		printf("equals\n");
		return (f1.user_id == this->user_id && strcmp(f1.user_name, this->user_name) == 0 && strcmp(f1.user_password, this->user_password) == 0);
	}
};

class FConfigure{
public:
	int user_count = 0;
};

class FUserOP{
private :
	FUserInfo temp_login_info;
	FConfigure fconfigure;
	char user_database_file[32];
	char database_configure_file[32];
public:
	FUserOP(){
		strcpy(user_database_file, "user.db");
		strcpy(database_configure_file, "setup.db");
	}
	int save(FUserInfo *user_info){
		if (user_info == NULL)
			return 0;
		//�û��Ѿ�������
		if (login(user_info) == 1)
			return 2;
		//��ȡ����id
		FILE *file1 = fopen(database_configure_file, "rb+");
		if (file1 == NULL)
			return 0;
		//��ȡ��ǰ�û�������
		fseek(file1, 0, SEEK_SET);
		int len = fread(&fconfigure, 1, sizeof(FConfigure) , file1);
		
		fclose(file1);
		if (len != sizeof(FConfigure))
		{
			printf("error!\n");
			return 3;
		}
		//д������
		fconfigure.user_count = fconfigure.user_count + 1;
		user_info->user_id = fconfigure.user_count;
		printf("user id:%d\n", user_info->user_id);
		FILE *file2 = fopen(user_database_file, "ab");		//׷����Ϣ���ļ�ĩβ
		if (file2 != NULL)
		{
			fwrite(user_info, 1, sizeof(FUserInfo), file2);
			fclose(file2);
		}

		file1 = fopen(database_configure_file, "wb+");
		fseek(file1, 0, SEEK_SET);
		fwrite(&fconfigure, 1, sizeof(FConfigure), file1);
		fclose(file1);
		return 1;
	}
	int login(FUserInfo *login_info){
		if (login_info == NULL)
			return 0;
		FILE *file = fopen(user_database_file, "rb");
		if (file == NULL)
		{
			return 0;
		}
		while (true)
		{
			int nlen = fread(&temp_login_info, 1, sizeof(temp_login_info), file);
			//��ȡ���ݳɹ�
			if (nlen == sizeof(temp_login_info))
			{
				if (strcmp(temp_login_info.user_name, login_info->user_name) == 0 &&
					strcmp(temp_login_info.user_password, login_info->user_password) == 0)
				{
					//��ȡ�û�id
					login_info->user_id = temp_login_info.user_id;
					fclose(file);
					return 1;				//��¼�ɹ���
				}
			}
			else
				break;
		}
		fclose(file);
		return 0;
	}
};
#endif