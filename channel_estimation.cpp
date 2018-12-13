void Get_Lum_value(Mat& Lum_value) {
	loop = 0;
	while (loop < LOOP_TIMES) {	//ランダムに点灯するLEDを撮影した画像から各LEDの画素値を取得

		/*撮影画像の読み込み*/
		sprintf_s(image_name, FILE_PATH + file_name);

		cv::Mat src_image = cv::imread(image_name, 0); //画像の読み込み部分：0とすることで強制的にグレースケールでの読み込み。<0でそのまま読み込み。
		if (!src_image.data) {
			cout << "ファイルの読み込みに失敗しました。\n";
			cout << "読み込もうとしたファイル名は:" << image_name << "でした。\n";
		}
		for (int j = 0; j < NUMLED; j++) { //NUMLED: 合計LED数
			//画素値の格納: jはあるパターンiでのj番目の画素値。それがloop回数分格納される
			Lum_value.at<double>(j, loop) = src_image.at<uchar>(yled[j], xled[j]);
		}
		loop++;
	}
}

void Get_LED_pattern(Mat& LED_pattern) { // LEDの既知の点滅パターンを配列へ格納
	loop = 0;
	while (loop < LOOP_TIMES) { //LOOP_TIMES: 読み込みたい撮影画像の枚数
		
		/*************************************************************
		*ここで、LOOP_TIMES分の点滅パターンをled_pattern[j]に格納しておく.
		*ランダムパターンの生成式を書いてもよし、Textファイルに事前にパター
		*ンを保存しておき、それを読み込ませてもよい.
		**************************************************************/
		
		for (int j = 0; j < NUMLED + 1; j++) {
			if (j == 16) LED_pattern.at<double>(loop, j) = 1; //オフセット
			else LED_pattern.at<double>(loop, j) = led_pattern[j]; //既知のLEDの点滅パターンを格納
		}
		loop++;
	}
}

void Get_channel_estimation(Mat& GaussEffect_with_offset) { 
	//撮影画像のオフセット成分含めたチャネル推定
	//シミュレーションの場合、オフセットは特に考慮しなくてよい
	Get_LED_pattern(LED_pattern); //LED点滅パターン (LED数、撮影画像枚数)の行列
	Get_Lum_value(Lum_value); //撮影画像の画素値 (画素数, 撮影画像枚数)の行列
	invLED_pattern = LED_pattern.inv(DECOMP_SVD); //点滅パターンの擬似逆行列
	for (int i = 0; i < NUMLED; i++) { 
		for (int j = 0; j < TOTAL_PATTERN; j++) { //TOTAL_PATTERN: 撮影画像枚数 == LOOP_TIMES
			Lum_value02(j,0) = Lum_value(i, j);
		}
		Channel_est = invLED_pattern * Lum_value02; //画素値と既知の点滅パターンからチャネル推定
		for (int j = 0; j < NUMLED+1; j++) {
			GaussEffect_with_offset.at<double>(i,j) = Channel_est(j, 0);
		}
	}
}

