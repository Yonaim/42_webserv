#include "HTTP/const_values.hpp"
#include <map>

const std::pair<std::string, std::string> _MIME_TYPE[] = {
	std::pair<std::string, std::string>("html", "text/html"),
	std::pair<std::string, std::string>("htm", "text/html"),
	std::pair<std::string, std::string>("shtml", "text/html"),
	std::pair<std::string, std::string>("css", "text/css"),
	std::pair<std::string, std::string>("xml", "text/xml"),
	std::pair<std::string, std::string>("gif", "image/gif"),
	std::pair<std::string, std::string>("jpeg", "image/jpeg"),
	std::pair<std::string, std::string>("jpg", "image/jpeg"),
	std::pair<std::string, std::string>("js", "application/javascript"),
	std::pair<std::string, std::string>("atom", "application/atom+xml"),
	std::pair<std::string, std::string>("rss", "application/rss+xml"),

	std::pair<std::string, std::string>("mml", "text/mathml"),
	std::pair<std::string, std::string>("txt", "text/plain"),
	std::pair<std::string, std::string>("jad",
										"text/vnd.sun.j2me.app-descriptor"),
	std::pair<std::string, std::string>("wml", "text/vnd.wap.wml"),
	std::pair<std::string, std::string>("htc", "text/x-component"),

	std::pair<std::string, std::string>("png", "image/png"),
	std::pair<std::string, std::string>("svg", "image/svg+xml"),
	std::pair<std::string, std::string>("svgz", "image/svg+xml"),
	std::pair<std::string, std::string>("tif", "image/tiff"),
	std::pair<std::string, std::string>("tiff", "image/tiff"),
	std::pair<std::string, std::string>("wbmp", "image/vnd.wap.wbmp"),
	std::pair<std::string, std::string>("webp", "image/webp"),
	std::pair<std::string, std::string>("ico", "image/x-icon"),
	std::pair<std::string, std::string>("jng", "image/x-jng"),
	std::pair<std::string, std::string>("bmp", "image/x-ms-bmp"),

	std::pair<std::string, std::string>("woff", "image/woff"),
	std::pair<std::string, std::string>("woff2", "image/woff2"),

	std::pair<std::string, std::string>("jar", "application/java-archive"),
	std::pair<std::string, std::string>("war", "application/java-archive"),
	std::pair<std::string, std::string>("ear", "application/java-archive"),
	std::pair<std::string, std::string>("json", "application/json"),
	std::pair<std::string, std::string>("hqx", "application/mac-binhex40"),
	std::pair<std::string, std::string>("doc", "application/msword"),
	std::pair<std::string, std::string>("pdf", "application/pdf"),
	std::pair<std::string, std::string>("ps", "application/postscript"),
	std::pair<std::string, std::string>("eps", "application/postscript"),
	std::pair<std::string, std::string>("ai", "application/postscript"),
	std::pair<std::string, std::string>("rtf", "application/rtf"),
	std::pair<std::string, std::string>("m3u8",
										"application/vnd.apple.mpegurl"),
	std::pair<std::string, std::string>("kml",
										"application/vnd.google-earth.kml+xml"),
	std::pair<std::string, std::string>("kmz",
										"application/vnd.google-earth.kmz"),
	std::pair<std::string, std::string>("xls", "application/vnd.ms-excel"),
	std::pair<std::string, std::string>("eot",
										"application/vnd.ms-fontobject"),
	std::pair<std::string, std::string>("ppt",
										"application/vnd.ms-powerpoint"),
	std::pair<std::string, std::string>(
		"odg", "application/vnd.oasis.opendocument.graphics"),
	std::pair<std::string, std::string>(
		"odp", "application/vnd.oasis.opendocument.presentation"),
	std::pair<std::string, std::string>(
		"ods", "application/vnd.oasis.opendocument.spreadsheet"),
	std::pair<std::string, std::string>(
		"odt", "application/vnd.oasis.opendocument.text"),
	std::pair<std::string, std::string>(
		"pptx",
		"application/"
		"vnd.openxmlformats-officedocument.presentationml.presentation"),
	std::pair<std::string, std::string>("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"),
	std::pair<std::string, std::string>("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"),
	std::pair<std::string, std::string>("wmlc", "application/vnd.wap.wmlc"),
	std::pair<std::string, std::string>("7z", "application/x-7z-compressed"),
	std::pair<std::string, std::string>("cco", "application/x-cocoa"),
	std::pair<std::string, std::string>("jardiff", "application/x-java-archive-diff"),
	std::pair<std::string, std::string>("jnlp", "application/x-java-jnlp-file"),
	std::pair<std::string, std::string>("run", "application/x-makeself"),
	std::pair<std::string, std::string>("pl", "application/x-perl"),
	std::pair<std::string, std::string>("pm", "application/x-perl"),
	std::pair<std::string, std::string>("prc", "application/x-pilot"),
	std::pair<std::string, std::string>("pdb", "application/x-pilot"),
	std::pair<std::string, std::string>("rar", "application/x-rar-compressed"),
	std::pair<std::string, std::string>("rpm", "application/x-redhat-package-manager"),
	std::pair<std::string, std::string>("sea", "application/x-sea"),
	std::pair<std::string, std::string>("swf", "application/x-shockwave-flash"),
	std::pair<std::string, std::string>("sit", "application/x-stuffit"),
	std::pair<std::string, std::string>("tcl", "application/x-tcl"),
	std::pair<std::string, std::string>("tk", "application/x-tcl"),
	std::pair<std::string, std::string>("der", "application/x-x509-ca-cert"),
	std::pair<std::string, std::string>("pem", "application/x-x509-ca-cert"),
	std::pair<std::string, std::string>("crt", "application/x-x509-ca-cert"),
	std::pair<std::string, std::string>("xpi", "application/x-xpinstall"),
	std::pair<std::string, std::string>("xhtml", "application/xhtml+xml"),
	std::pair<std::string, std::string>("xspf", "application/xspf+xml"),
	std::pair<std::string, std::string>("zip", "application/zip"),

	std::pair<std::string, std::string>("bin", "application/octet-stream"),
	std::pair<std::string, std::string>("exe", "application/octet-stream"),
	std::pair<std::string, std::string>("dll", "application/octet-stream"),
	std::pair<std::string, std::string>("deb", "application/octet-stream"),
	std::pair<std::string, std::string>("dmg", "application/octet-stream"),
	std::pair<std::string, std::string>("iso", "application/octet-stream"),
	std::pair<std::string, std::string>("img", "application/octet-stream"),
	std::pair<std::string, std::string>("msi", "application/octet-stream"),
	std::pair<std::string, std::string>("msp", "application/octet-stream"),
	std::pair<std::string, std::string>("msm", "application/octet-stream"),

	std::pair<std::string, std::string>("mid", "audio/midi"),
	std::pair<std::string, std::string>("midi", "audio/midi"),
	std::pair<std::string, std::string>("kar", "audio/midi"),
	std::pair<std::string, std::string>("mp3", "audio/mpeg"),
	std::pair<std::string, std::string>("ogg", "audio/ogg"),
	std::pair<std::string, std::string>("m4a", "audio/x-m4a"),
	std::pair<std::string, std::string>("ra", "audio/x-realaudio"),

	std::pair<std::string, std::string>("3gpp", "video/3gpp"),
	std::pair<std::string, std::string>("3gp", "video/3gpp"),
	std::pair<std::string, std::string>("ts", "video/mp2t"),
	std::pair<std::string, std::string>("mp4", "video/mp4"),
	std::pair<std::string, std::string>("mpeg", "video/mpeg"),
	std::pair<std::string, std::string>("mpg", "video/mpeg"),
	std::pair<std::string, std::string>("mov", "video/quicktime"),
	std::pair<std::string, std::string>("webm", "video/webm"),
	std::pair<std::string, std::string>("flv", "video/x-flv"),
	std::pair<std::string, std::string>("m4v", "video/x-m4v"),
	std::pair<std::string, std::string>("mng", "video/x-mng"),
	std::pair<std::string, std::string>("asx", "video/x-ms-asf"),
	std::pair<std::string, std::string>("asf", "video/x-ms-asf"),
	std::pair<std::string, std::string>("wmv", "video/x-ms-wmv"),
	std::pair<std::string, std::string>("avi", "video/x-msvideo"),
};

const std::map<std::string, std::string> HTTP::MIME_TYPE(
	_MIME_TYPE, _MIME_TYPE + sizeof(_MIME_TYPE) / sizeof(_MIME_TYPE[0]));
