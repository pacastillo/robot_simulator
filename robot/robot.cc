#include <math.h>
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include <gdkmm/colormap.h>
#include <gdkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/window.h>
#include <gtkmm/main.h>
#include <gtkmm/dialog.h>
#include <cmath>
#include <sstream>
#include <vector>
#include <pangomm/fontdescription.h>
#include "sincronizador.h"

//*****************************************************************************

#ifndef M_PI
#define M_PI 3.14159265359
#endif /* M_PI */
#define DEG2RAD .01745329251994444444   // (M_PI/180)

#define def_APERTURA_SENSORES 50
#define def_EXCITACION_MAX  20


//*****************************************************************************

class MyDialog : public Gtk::Dialog {
public:
  // create one dialog only
  static MyDialog* instance() {
    if( _instance == 0 ) {
      _instance = new MyDialog;
    }
    return _instance;
  }

protected:
  MyDialog() {
    //set_size_request(300, 200);
    set_resizable(false);
    set_title("Acerca de...");

    Gtk::Label *label1 = manage(new Gtk::Label("Simulador de Robot v1.0",true));
    get_vbox()->pack_start(*label1);
    label1->show();

    Gtk::Label *label20 = manage(new Gtk::Label(" ",true));
    get_vbox()->pack_start(*label20);
    label20->show();
    Gtk::Label *label21 = manage(new Gtk::Label("          Simulador de robot para las practicas          ",true));
    get_vbox()->pack_start(*label21);
    label21->show();
    Gtk::Label *label22 = manage(new Gtk::Label("de Estructura de los Computadores I",true));
    get_vbox()->pack_start(*label22);
    label22->show();
    Gtk::Label *label23 = manage(new Gtk::Label("de Ingenieria Informatica",true));
    get_vbox()->pack_start(*label23);
    label23->show();

    Gtk::Label *label3 = manage(new Gtk::Label(" ",true));
    get_vbox()->pack_start(*label3);
    label3->show();
    Gtk::Label *label4 = manage(new Gtk::Label("(c) Pedro A. Castillo Valdivieso",true));
    get_vbox()->pack_start(*label4);
    label4->show();
    Gtk::Label *label5 = manage(new Gtk::Label(" ",true));
    get_vbox()->pack_start(*label5);
    label5->show();

    Gtk::Label *label6 = manage(new Gtk::Label("Universidad de Granada",true));
    get_vbox()->pack_start(*label6);
    label6->show();
    Gtk::Label *label7 = manage(new Gtk::Label(" ",true));
    get_vbox()->pack_start(*label7);
    label7->show();

    Gtk::Button *button=manage(new Gtk::Button("Cerrar"));
    get_action_area()->pack_start(*button);
    button->signal_clicked().connect(SigC::slot(*this, &Widget::hide));
    button->show();
  }
  
  
private:
  static MyDialog* _instance;
};

MyDialog* MyDialog::_instance(0);



//*****************************************************************************

class Robot {
public:
  int x,y,a;
  Robot(unsigned _x, unsigned _y, unsigned _a) {
    x=_x;
    y=_y;
    a=_a;
  }
  virtual ~Robot() {}

  // recibe valores ENTEROS de movim en [0,10]  (desde la Ventana)
  void movimiento(unsigned _izq, unsigned _dcha) {
    double radian= a * DEG2RAD ;

    double tmp = (double)( cos(radian)*(1*(_izq+_dcha)/2) );

    x = x + (unsigned)tmp;

    tmp = (double)( sin(radian)*(1*(_izq+_dcha)/2) );

    y = y - (unsigned)tmp;

    //cout <<"   x="<<x<<"\n\ttmpY="<<tmp<<"   y="<<y<<endl;
  }

  // izq(direcc==+1)  dcha(direcc==-1)
  void girar(int _direcc, unsigned _grados) {
    a = (a + (_direcc * _grados) );
    a = a % 360;
    a = (a<0) ? (360+a) : (a);
    double radian= a * DEG2RAD ;
    x = x + (unsigned)(cos(radian) * 5 );
    y = y - (unsigned)(sin(radian) * 5 );
  }

};

//*****************************************************************************

class Arena : public Gtk::DrawingArea {
public:
  Arena();
  virtual ~Arena();
  bool timer_callback();
  void avanzarRobot(unsigned _izq, unsigned _dcha);
  void girarRobot(int _direcc, unsigned _grados);
  std::vector<unsigned> verObstaculos();
  bool esObstaculo(unsigned _x, unsigned _y);
  void mostrar_informacion();
  void incrementar_pasos();
  void cambiar_escenario();
  void obstaculo_movil();
  void ctrl_reiniciar();
  std::vector<unsigned> ctrl_datos_posicion();

protected:
  // manejadores de eventos
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);

  Glib::RefPtr<Gdk::GC> gc_;
  Gdk::Color blue_, red_, green_, black_, white_, grey_, yellow_;
  Glib::RefPtr<Gdk::Colormap> colormap;

private:
  Robot robot;
  unsigned xT,yT;
  unsigned EXCITACION_MAX;
  unsigned escenario,pasos,numChoques;
  int mover_obstaculos,increm_mover_obstaculos;
};


Arena::Arena()
        :robot(30,30,0) {
  EXCITACION_MAX = def_EXCITACION_MAX ;

  // inicializar colores; el resto se hace en   on_realize()
  colormap = get_default_colormap();

  blue_ = Gdk::Color("blue");
  red_ = Gdk::Color("red");
  green_ = Gdk::Color("green");

  black_ = Gdk::Color("black");
  white_ = Gdk::Color("white");
  grey_ = Gdk::Color("grey");

  yellow_ = Gdk::Color("yellow");

  colormap->alloc_color(blue_);
  colormap->alloc_color(red_);
  colormap->alloc_color(green_);
  
  colormap->alloc_color(black_);
  colormap->alloc_color(white_);
  colormap->alloc_color(grey_);

  colormap->alloc_color(yellow_);

  // timeout
  //Glib::signal_timeout().connect(SigC::slot(*this, &Arena::timer_callback), 500);

  add_events(Gdk::EXPOSURE_MASK);

  escenario = 3;
  pasos = 0;
  numChoques=0;
  mover_obstaculos = 0;
  increm_mover_obstaculos = 0;
}


Arena::~Arena() {}


void Arena::on_realize() {
  Gtk::DrawingArea::on_realize();

  // inicializar el resto de cosas
  Glib::RefPtr<Gdk::Window> window = get_window();

  gc_ = Gdk::GC::create(window);

  window->set_background(white_);
  window->clear();

  gc_->set_foreground(black_);
}


bool Arena::on_expose_event(GdkEventExpose*) {
  Glib::RefPtr<Gdk::Window> window = get_window();

  // obtener los tamaños de ventana para "repintar" en función de estos
  int winx, winy, winw, winh, wind;
  window->get_geometry(winx, winy, winw, winh, wind);

  window->clear();

  /*
  // la malla, para definir cuadritos:
  gc_->set_foreground(grey_);
  gc_->set_line_attributes(1,Gdk::LINE_SOLID,Gdk::CAP_NOT_LAST,Gdk::JOIN_MITER);
  for(unsigned fila=0; fila<(unsigned)winw; fila+=10){
    for(unsigned colum=0; colum<(unsigned)winh; colum+=10){
      window->draw_line(gc_, fila, 1, fila, winh-1);
      window->draw_line(gc_, 1, colum, winw-1, colum);
    }
  }
  */


  // poner LOS BORDES
  gc_->set_foreground(blue_);
  window->draw_rectangle(gc_, 1, 0,0, winw,12);
  window->draw_rectangle(gc_, 1, 0,0, 10,winh);
  window->draw_rectangle(gc_, 1, 0,winh-10, winw,10);
  window->draw_rectangle(gc_, 1, winw-10,0, 10,winh);

  switch( escenario ) {
  case 0:  // poner OBSTACULOS (escenario 1)
      window->draw_rectangle(gc_, 1, (winw/3)+30,0, 10,winh/2);
      window->draw_rectangle(gc_, 1, (winw/5)-30,winh/3, 2*winw/3,10);
      window->draw_rectangle(gc_, 1, winw-(winw/3),winh/3+mover_obstaculos*winh/80 , 10,5*winh/9);
      window->draw_rectangle(gc_, 1, winw/3,winh-(winh/3), 12*winw/20,10);
      window->draw_rectangle(gc_, 1, winw/5,winh/2, 10,2*winh/3);
      gc_->set_foreground(green_);  //OBJETIVO
      window->draw_rectangle(gc_, 1, winw/2, 10, winw/25,winh/25); 
      xT=winw/2; yT=10;
      break;
  case 1:  // poner OBSTACULOS (escenario 2)
      window->draw_rectangle(gc_, 1, 0,winh/5, winw,10);
      window->draw_rectangle(gc_, 1, 0,winh/2, winw,10);
      gc_->set_foreground(white_);
      window->draw_rectangle(gc_, 1, winw/2,50, winw/20,winh/2);
      gc_->set_foreground(blue_);
      window->draw_rectangle(gc_, 1, winw/6,winh/3, winw,10);
      window->draw_rectangle(gc_, 1, winw/6,2*winh/3, 2*winw/3,winh/5);
      window->draw_rectangle(gc_, 1, 0,2*winh/3, winw/6,20);
      gc_->set_foreground(green_);  //OBJETIVO
      window->draw_rectangle(gc_, 1, winw/6 - 10,3*winh/4, winw/25,winh/25);
      xT=winw/6 -10; yT=3*winh/4;
      break;
  case 2:  // poner OBSTACULOS (escenario 3)
      window->draw_rectangle(gc_, 1, 0,winh/4, winw,winh/2);
      window->draw_rectangle(gc_, 1, 9*winw/20,0, 3*winw/20,18*winh/20);

      window->draw_rectangle(gc_, 1, winw/7+mover_obstaculos*winw/80   , winh/9,       winw/15,winh/15);
      window->draw_rectangle(gc_, 1, winw/5-mover_obstaculos*winw/80   , 17*winh/20,   winw/15,winh/15);
      window->draw_rectangle(gc_, 1, 4*winw/6+mover_obstaculos*winw/80 , winh/9,     winw/15,winh/15);
      window->draw_rectangle(gc_, 1, 4*winw/5-mover_obstaculos*winw/80 , 17*winh/20, winw/15,winh/15);

      gc_->set_foreground(white_);
      window->draw_rectangle(gc_, 1, winw/5,winh/4, winw/20,winh/2);
      window->draw_rectangle(gc_, 1, 4*winw/5,20+winh/4, winw/20,winh/2);
      window->draw_rectangle(gc_, 1, winw/2,winh/4, 3*winw/20,winh/2);
      gc_->set_foreground(green_);  //OBJETIVO
      window->draw_rectangle(gc_, 1, winw - winw/25 -10, 20, winw/25,winh/25);
      xT=winw - winw/25 -10; yT=20;
      break;
  case 3:
      window->draw_rectangle(gc_, 1, winw/6,winh/6, winw/3,winh/3);
      window->draw_rectangle(gc_, 1, winw/3,15*winh/24, winw/4,winh/4);
      window->draw_rectangle(gc_, 1, 15*winw/24,winh/3, winw/4,winh/4);
      gc_->set_foreground(green_);  //OBJETIVO
      window->draw_rectangle(gc_, 1, winw - winw/25 -10, winh - winh/10, winw/25,winh/25);
      xT=winw - winw/25 -10; yT=winh - winh/10;
      break;
  }
  if( increm_mover_obstaculos ){
    mover_obstaculos += increm_mover_obstaculos;
    if( mover_obstaculos < 1 ) {
      increm_mover_obstaculos = 1 ;
      mover_obstaculos = 1 ;
    }
    if( mover_obstaculos > 15 ) {
      increm_mover_obstaculos = -1 ;
    }
  }


  unsigned x = robot.x;
  unsigned y = robot.y;
  int alfa = robot.a;
  int radio;
  if (winw>winh) radio=winh/25; else radio=winw/25;

  //COMPROBAR SI SE HA SALIDO POR LA DERECHA O ABAJO PORQUE HAYAN HECHO MÁS PEQUEÑA LA VENTANA DE REPENTE
  if( (int)x > winw ){
    x = winw-radio;
    robot.x = x;
  }
  if( (int)y > winh ){
    y = winh-radio;
    robot.y = y;
  }

  /*
  // el alcance de los sensores
  gc_->set_foreground(grey_);
  gc_->set_line_attributes(3,Gdk::LINE_ON_OFF_DASH,Gdk::CAP_NOT_LAST,Gdk::JOIN_MITER );
  window->draw_arc(gc_,0,  x-(radio/2),y-(radio/2),  radio,radio,  64*(alfa-def_APERTURA_SENSORES), 64*15);
  window->draw_arc(gc_,0,  x-(radio/2),y-(radio/2),  radio,radio,  64*(alfa-10), 64*20);
  window->draw_arc(gc_,0,  x-(radio/2),y-(radio/2),  radio,radio,  64*(alfa+def_APERTURA_SENSORES), 64*15);
  */
 
  gc_->set_foreground(black_);
  gc_->set_line_attributes(/*line_width*/1, /*LineStyle*/Gdk::LINE_SOLID, /*CapStyle*/Gdk::CAP_NOT_LAST, /*JoinStyle*/Gdk::JOIN_MITER );
  window->draw_arc(gc_,1,  x-(radio/2),y-(radio/2),  radio,radio,  0, 360*64);

  // los sensores, en rojo
  gc_->set_foreground(red_);
  gc_->set_line_attributes(/*line_width {4-2}*/ 4 , /*LineStyle*/Gdk::LINE_SOLID, /*CapStyle*/Gdk::CAP_NOT_LAST, /*JoinStyle*/Gdk::JOIN_MITER );
  window->draw_arc(gc_,0,  x-(radio/2),y-(radio/2),  radio,radio,  64*(alfa-def_APERTURA_SENSORES), 64*15);
  window->draw_arc(gc_,0,  x-(radio/2),y-(radio/2),  radio,radio,  64*(alfa-10), 64*20);
  window->draw_arc(gc_,0,  x-(radio/2),y-(radio/2),  radio,radio,  64*(alfa+def_APERTURA_SENSORES), 64*15);

/*
  gc_->set_foreground(red_);
  window->draw_point(gc_, x,y);
  window->draw_point(gc_, x+(unsigned)(cos(alfa*DEG2RAD)*(1+radio/2)),y-(unsigned)(sin(alfa*DEG2RAD)*(1+radio/2)) );
  window->draw_point(gc_, x+(unsigned)(cos((alfa-30)*DEG2RAD)*(1+radio/2)),y-(unsigned)(sin((alfa-30)*DEG2RAD)*(1+radio/2)) );
  window->draw_point(gc_, x+(unsigned)(cos((alfa+30)*DEG2RAD)*(1+radio/2)),y-(unsigned)(sin((alfa+30)*DEG2RAD)*(1+radio/2)) );

  gc_->set_foreground(black_);
  window->draw_point(gc_, x,y);
  window->draw_point(gc_, x+(unsigned)(cos(alfa*DEG2RAD)*(1+radio/2)),y-(unsigned)(sin(alfa*DEG2RAD)*(1+radio/2)) );
  window->draw_point(gc_, x+(unsigned)(cos((alfa-45)*DEG2RAD)*(1+radio/2)),y-(unsigned)(sin((alfa-45)*DEG2RAD)*(1+radio/2)) );
  window->draw_point(gc_, x+(unsigned)(cos((alfa+45)*DEG2RAD)*(1+radio/2)),y-(unsigned)(sin((alfa+45)*DEG2RAD)*(1+radio/2)) );
*/

  // texto "(x,y) = "
  gc_->set_foreground(white_);  
  std::ostringstream ostr;  ostr << "(x,y) = (" << x << " , " << y << ")   alfa="<<alfa<<"   PASOS="<<pasos<<"   CHOQUES="<<numChoques ;
  Glib::ustring ustr = ostr.str();
  Glib::RefPtr<Pango::Layout> texto = create_pango_layout(ustr);
  window->draw_layout(gc_,  2,0,  texto);


  //////////////////////////////////////////////////////////
  // los alcances de los sensores en AMARILLO
  gc_->set_foreground(green_);
  gc_->set_line_attributes(2 , Gdk::LINE_SOLID, Gdk::CAP_NOT_LAST, Gdk::JOIN_MITER );
  x = robot.x;
  y = robot.y;
  double radian= (robot.a + def_APERTURA_SENSORES) * DEG2RAD ;
  x = x + (unsigned)( cos(radian)*(EXCITACION_MAX+2 + radio/2) );
  y = y - (unsigned)( sin(radian)*(EXCITACION_MAX+2 + radio/2) );
  window->draw_line(gc_, x,y, x+1,y+1);
  x = robot.x;
  y = robot.y;
  radian= robot.a * DEG2RAD ;
  x = x + (unsigned)( cos(radian)*(EXCITACION_MAX+2 + radio/2) );
  y = y - (unsigned)( sin(radian)*(EXCITACION_MAX+2 + radio/2) );
  window->draw_line(gc_, x,y, x+1,y+1);
  x = robot.x;
  y = robot.y;
  radian= (robot.a - def_APERTURA_SENSORES) * DEG2RAD ;
  x = x + (unsigned)( cos(radian)*(EXCITACION_MAX+2 + radio/2) );
  y = y - (unsigned)( sin(radian)*(EXCITACION_MAX+2 + radio/2) );
  window->draw_line(gc_, x,y, x+1,y+1);
  //////////////////////////////////////////////////////////


  return true;
}


bool Arena::timer_callback() {
 queue_draw();
 // si dev=1 -> seguir el callback ; si dev=0 -> no se llama más
 return 1;
}


void Arena::avanzarRobot(unsigned _izq, unsigned _dcha) {
  unsigned tmp_x = robot.x;
  unsigned tmp_y = robot.y;

  robot.movimiento( _izq, _dcha );

  int winx, winy, winw, winh, wind;
  get_window()->get_geometry(winx, winy, winw, winh, wind);
  unsigned radio;
  if (winw>winh) radio=winh/25; else radio=winw/25;

  if(robot.x<10){
      robot.x = tmp_x+5;
      cout << "\t\t (robot) AVISO: ha intentado SALIRSE de la arena AL AVANZAR..." << endl;
  }
  if(robot.y<10){
      robot.y = tmp_y+5;
      cout << "\t\t (robot) AVISO: ha intentado SALIRSE de la arena AL AVANZAR..." << endl;
  }
  if(robot.x>winw-10){
      robot.x = tmp_x-5;
      cout << "\t\t (robot) AVISO: ha intentado SALIRSE de la arena AL AVANZAR..." << endl;
  }
  if(robot.y>winh-10){
      robot.y = tmp_y-5;
      cout << "\t\t (robot) AVISO: ha intentado SALIRSE de la arena AL AVANZAR..." << endl;
  }

  unsigned x = robot.x;
  unsigned y = robot.y;
  double radian= robot.a * DEG2RAD ;
  x = x + (unsigned)( cos(radian)*(2+radio/2) );
  y = y - (unsigned)( sin(radian)*(2+radio/2) );
  if ( esObstaculo(x,y) ) {
      robot.x = tmp_x;
      robot.y = tmp_y;
      cout << "\t\t (robot) AVISO: ha intentado atravesar la pared por el centro AL AVANZAR..." << endl;
      numChoques++;
  }

  queue_draw();
}


void Arena::girarRobot(int _direcc, unsigned _grados) {
  unsigned tmp_x = robot.x;
  unsigned tmp_y = robot.y;

  robot.girar( _direcc, _grados );

  int winx, winy, winw, winh, wind;
  get_window()->get_geometry(winx, winy, winw, winh, wind);
  unsigned radio;
  if (winw>winh) radio=winh/25; else radio=winw/25;

  if(robot.x<10){
      robot.x = tmp_x+5;
      cout << "\t\t (robot) AVISO: ha intentado SALIRSE de la arena AL AVANZAR..." << endl;
  }
  if(robot.y<10){
      robot.y = tmp_y+5;
      cout << "\t\t (robot) AVISO: ha intentado SALIRSE de la arena AL AVANZAR..." << endl;
  }
  if(robot.x>winw-10){
      robot.x = tmp_x-5;
      cout << "\t\t (robot) AVISO: ha intentado SALIRSE de la arena AL AVANZAR..." << endl;
  }
  if(robot.y>winh-10){
      robot.y = tmp_y-5;
      cout << "\t\t (robot) AVISO: ha intentado SALIRSE de la arena AL AVANZAR..." << endl;
  }

  unsigned x = robot.x;
  unsigned y = robot.y;
  double radian= robot.a * DEG2RAD ;
  x = x + (unsigned)( cos(radian)*(2+radio/2) );
  y = y - (unsigned)( sin(radian)*(2+radio/2) );
  if ( esObstaculo(x,y) ) {
      robot.x = tmp_x;
      robot.y = tmp_y;
      cout << "\t\t (robot) AVISO: ha intentado atravesar la pared por el centro EN EL GIRO..." << endl;
      numChoques++;
  }

  queue_draw();
}


bool Arena::esObstaculo(unsigned _x, unsigned _y) {
  Glib::RefPtr<Gdk::Pixbuf> p = 
    Gdk::Pixbuf::create( (Glib::RefPtr<Gdk::Drawable>)get_window(), colormap, _x,_y, 0,0, 1,1);
  
  int r = (int) p->get_pixels()[0] ;
  int g = (int) p->get_pixels()[1] ;
  int b = (int) p->get_pixels()[2] ;
  //cout << "\t ("<<_x<<","<<_y<<")="<< r <<" "<< g <<" "<< b ;

  return ( (!r)&&(!g)&&(b) ) ;  // encontrado un pixel azul => obstaculo
}

//sigue las direcciones de los 3 sensores viendo el color de los pixels
//y los devuelve en el vector<di,dc,dd>
//di,dc,dd son valores enteros positivos.
//si dx==0   => no hay obstaculo a la vista
//si 0<dx<=1 => hay obstaculo a la vista
std::vector<unsigned> Arena::verObstaculos() {
  int winx, winy, winw, winh, wind;
  get_window()->get_geometry(winx, winy, winw, winh, wind);
  unsigned i,radio;
  if (winw>winh) radio=winh/25; else radio=winw/25;

  std::vector<unsigned> dev;
  dev.push_back(0); // izq
  dev.push_back(0); // centro
  dev.push_back(0); // dcha
  dev.push_back(0); // dist
  dev.push_back(0); // ang

  //cout << endl << "\n -- OBSTACULO izq:" << endl;
  for(i=0; i<EXCITACION_MAX; i++) {
    unsigned x = robot.x;
    unsigned y = robot.y;
    double radian= (robot.a + def_APERTURA_SENSORES) * DEG2RAD ;
    x = x + (unsigned)( cos(radian)*(i+1+radio/2) );
    y = y - (unsigned)( sin(radian)*(i+1+radio/2) );

    Glib::RefPtr<Gdk::Pixbuf> p = 
      Gdk::Pixbuf::create( (Glib::RefPtr<Gdk::Drawable>)get_window(), colormap, x,y, 0,0, 1,1);

    int r = (int) p->get_pixels()[0] ;
    int g = (int) p->get_pixels()[1] ;
    int b = (int) p->get_pixels()[2] ;
    //cout << "\t ("<<x<<","<<y<<")="<< r <<" "<< g <<" "<< b ;
    if( (!r)&&(!g)&&(b) ) {  // encontrado un pixel azul => obstaculo
      dev[0] = EXCITACION_MAX - i ;
      break;
    }
    //if( (!r)&&(g)&&(!b) && (i<10)) {  // encontrado un pixel VERDE => meta
    //return (std::vector<unsigned>)meta;
    //}
  }

  //cout << endl << "\n -- OBSTACULO centro:" << endl;
  for(i=0; i<EXCITACION_MAX; i++) {
    unsigned x = robot.x;
    unsigned y = robot.y;
    double radian= robot.a * DEG2RAD ;
    x = x + (unsigned)( cos(radian)*(i+1+radio/2) );
    y = y - (unsigned)( sin(radian)*(i+1+radio/2) );

    Glib::RefPtr<Gdk::Pixbuf> p = 
      Gdk::Pixbuf::create( (Glib::RefPtr<Gdk::Drawable>)get_window(), colormap, x,y, 0,0, 1,1);

    int r = (int) p->get_pixels()[0] ;
    int g = (int) p->get_pixels()[1] ;
    int b = (int) p->get_pixels()[2] ;
    //cout << "\t ("<<x<<","<<y<<")="<< r <<" "<< g <<" "<< b ;
    if( (!r)&&(!g)&&(b) ) {  // encontrado un pixel azul => obstaculo
      dev[1] = EXCITACION_MAX - i ;
      break;
    }
    //if( (!r)&&(g)&&(!b) && (i<10)) {  // encontrado un pixel VERDE => meta
    //  return (std::vector<unsigned>)meta;
    //}
  }

  //cout << endl << "\n -- OBSTACULO dcha:" << endl;
  for(i=0; i<EXCITACION_MAX; i++) {
    unsigned x = robot.x;
    unsigned y = robot.y;
    double radian= (robot.a - def_APERTURA_SENSORES) * DEG2RAD ;
    x = x + (unsigned)( cos(radian)*(i+1+radio/2) );
    y = y - (unsigned)( sin(radian)*(i+1+radio/2) );

    Glib::RefPtr<Gdk::Pixbuf> p = 
      Gdk::Pixbuf::create( (Glib::RefPtr<Gdk::Drawable>)get_window(), colormap, x,y, 0,0, 1,1);

    int r = (int) p->get_pixels()[0] ;
    int g = (int) p->get_pixels()[1] ;
    int b = (int) p->get_pixels()[2] ;
    //cout << "\t ("<<x<<","<<y<<")="<< r <<" "<< g <<" "<< b ;
    if( (!r)&&(!g)&&(b) ) {  // encontrado un pixel azul => obstaculo
      dev[2] = EXCITACION_MAX - i ;
      break;
    }
    //if( (!r)&&(g)&&(!b) && (i<10)) {  // encontrado un pixel VERDE => meta
    //  return (std::vector<unsigned>)meta;
    //}
  }

  double x = (double) robot.x;
  double y = (double) robot.y;
  //calcular la distancia al obj  =>  sqrt( (x-xT)^2 + (y-yT)^2 )
  double tmpx = (double) (x-xT)*(x-xT);
  double tmpy = (double) (y-yT)*(y-yT);
  tmpx = (double) sqrt( tmpx + tmpy );
  dev[3] = (unsigned) tmpx;

  ////calcular el angulo relativo////
  if(x == xT){   // => aRT=90
    if(y<yT){
      tmpy = (double) ( robot.a - 90.0 );
    }else{
      tmpy = (double) ( robot.a - 90.0 );
      tmpy = (double) ( 360.0 - tmpy );
    }
  }else{
    //calcular el angulo respecto al obj  =>   ra - atan((y-yT)/(x-xT)) 
    tmpx = (double) atan( (y-yT) / (x-xT) );
    tmpx = (double) tmpx * 57.29577951307854999853;

    //cout << "\n\t\t\txR="<<x<<"  xT="<<xT<<"  /  aR="<<robot.a<<"  aRT="<<tmpx<<endl;

    if(x<xT) {
      tmpy = (double) ( robot.a + tmpx );
      if(tmpy<0) tmpy = (double) ( 360.0 + tmpy );
    }else{
      tmpy = (double) ( ( robot.a - 180.0) + tmpx );
      if(tmpy<0) tmpy = (double) ( 360.0 + tmpy );
    }

  }    

  dev[4] = (unsigned) tmpy;

  return (std::vector<unsigned>)dev;
}

void Arena::mostrar_informacion() {
  cout << endl << "\n (robot) GEOMETRIA DE LA VENTANA:";
  int winx, winy, winw, winh, wind;
  get_window()->get_geometry(winx, winy, winw, winh, wind);
  unsigned radio;
  if (winw>winh) radio=winh/25; else radio=winw/25;
  cout << "\n\twinx = "<<winx
       << "\n\twiny = "<<winy
       << "\n\twinw = "<<winw
       << "\n\twinh = "<<winh;

  cout << endl << "POSICION DEL ROBOT:";
  cout << "\n\tx = "<<robot.x
       << "\n\ty = "<<robot.y
       << "\n\ta = "<<robot.a
       << "\n\tradio = "<<radio
       << "\n\tpasos = "<<pasos
       << "\n\tchoques = "<<numChoques;

  cout << endl << "POSICION DEL OBJETIVO:";
  cout << "\n\txT = "<<xT
       << "\n\tyT = "<<yT;
  double x = (double) robot.x;
  double y = (double) robot.y;
  double tmpx = (double) (x-xT)*(x-xT);
  double tmpy = (double) (y-yT)*(y-yT);
  tmpx = (double) sqrt( tmpx + tmpy );
  cout << "\n\tdistancia = "<<tmpx;

  ////calcular el angulo relativo////
  if(x == xT){   // => aRT=90
    if(y<yT){
      tmpy = (double) ( robot.a - 90.0 );
    }else{
      tmpy = (double) ( robot.a - 90.0 );
      tmpy = (double) ( 360.0 - tmpy );
    }
  }else{
    //calcular el angulo respecto al obj  =>   ra - atan((y-yT)/(x-xT)) 
    tmpx = (double) atan( (y-yT) / (x-xT) );
    tmpx = (double) tmpx * 57.29577951307854999853;

    if(x<xT) {
      tmpy = (double) ( robot.a - tmpx );
      if(tmpy<0) tmpy = (double) ( 360.0 + tmpy );
    }else{
      tmpy = (double) ( ( robot.a - 180.0) + tmpx );
      if(tmpy<0) tmpy = (double) ( 360.0 + tmpy );
    }

  }    
  cout << "\n\tangulo = "<<tmpy;

  cout << endl << "ESCENARIO:";
  cout << "\n\tescenario n. = "<<escenario
       << "\n\tobstaculos moviles = "<<increm_mover_obstaculos << endl ;
}

void Arena::incrementar_pasos() {
  pasos++;
}

void  Arena::cambiar_escenario() {
  escenario++;
  if(escenario>3) escenario=0;

  mover_obstaculos = 0;
  increm_mover_obstaculos = 0;

  queue_draw();
}

void Arena::obstaculo_movil() {
  if( increm_mover_obstaculos ) {
    increm_mover_obstaculos = 0 ;
  }else{
    increm_mover_obstaculos = 1 ;
  }
}

void Arena::ctrl_reiniciar() {
  mover_obstaculos = 0;
  increm_mover_obstaculos = 0;

  pasos = 0;
  numChoques=0;

  robot.x = 30;
  robot.y = 30;
  robot.a = 0;

  queue_draw();
}

// DEVOLVEMOS: distancia angulo choques pasos x y a xT yT
std::vector<unsigned> Arena::ctrl_datos_posicion() {
  std::vector<unsigned> dev;

  double x = (double) robot.x;
  double y = (double) robot.y;
  double tmpx = (double) (x-xT)*(x-xT);
  double tmpy = (double) (y-yT)*(y-yT);
  tmpx = (double) sqrt( tmpx + tmpy );

  dev.push_back( (unsigned)tmpx ); // distancia

  ////calcular el angulo relativo////
  if(x == xT){   // => aRT=90
    if(y<yT){
      tmpy = (double) ( robot.a - 90.0 );
    }else{
      tmpy = (double) ( robot.a - 90.0 );
      tmpy = (double) ( 360.0 - tmpy );
    }
  }else{
    //calcular el angulo respecto al obj  =>   ra - atan((y-yT)/(x-xT)) 
    tmpx = (double) atan( (y-yT) / (x-xT) );
    tmpx = (double) tmpx * 57.29577951307854999853;

    if(x<xT) {
      tmpy = (double) ( robot.a - tmpx );
      if(tmpy<0) tmpy = (double) ( 360.0 + tmpy );
    }else{
      tmpy = (double) ( ( robot.a - 180.0) + tmpx );
      if(tmpy<0) tmpy = (double) ( 360.0 + tmpy );
    }

  }    
  dev.push_back( (unsigned)tmpy ); // angulo

  dev.push_back( (unsigned)numChoques ); // choques

  dev.push_back(pasos);   // pasos

  dev.push_back(robot.x); // robot.x
  dev.push_back(robot.y); // robot.y
  dev.push_back(robot.a); // robot.a
  dev.push_back(xT);      // xT
  dev.push_back(yT);      // yT

  return (std::vector<unsigned>)dev;
}

//*****************************************************************************

class Ventana : public Gtk::Window {
public:
  Ventana();
  virtual ~Ventana();

  void pasoApaso();
  void continuo();
  void paso();
  void ejecutar_movimiento();
  void mostrar_informacion();
  void cambiar_escenario();
  void obstaculo_movil();
  void ctrl_reiniciar();
  void ctrl_datos_posicion();
  // la funcion idle
  bool callback();
  
protected:
  // Child widgets
  Gtk::VBox m_box0;
  Gtk::HBox m_box1;
  Gtk::VBox m_box2;
  Gtk::HBox m_box3; //empty box
  Gtk::HBox m_box4; //empty box

  Gtk::Button m_button1, m_button2, m_button3, m_button4, m_button5, m_button6, m_button7, m_button8;

  Arena m_area;

  bool ejecutar_continuo;
  bool ejecutando_movimiento;
  unsigned actuacion_izq;
  unsigned actuacion_dcha;
};


Ventana::Ventana()
  : m_box0(/*homogeneous*/false, /*spacing*/5), m_box1(false, 5), m_box2(false, 5), m_box3(false, 5), m_box4(false, 5),
    m_button1("Salir"), m_button2("Paso"), m_button3("Continuo"), m_button4("Informacion"), m_button5("Acerca de"), m_button6("Escenario"), 
    m_button7("Obst.movil"), m_button8("Reiniciar"), m_area(),
    ejecutar_continuo(false), ejecutando_movimiento(false)
{

  set_title("Simulador de Robot v1.0");

  // box2
  m_button1.signal_clicked().connect(SigC::slot(*this, &Gtk::Widget::hide));  // conectar con SALIR
  m_button2.signal_clicked().connect(SigC::slot(*this, &Ventana::pasoApaso));  // conectar con PASOaPASO
  m_button3.signal_clicked().connect(SigC::slot(*this, &Ventana::continuo));  // conectar con CONTINUO
  m_button4.signal_clicked().connect(SigC::slot(*this, &Ventana::mostrar_informacion));  // conectar con INFORM.
  m_button6.signal_clicked().connect(SigC::slot(*this, &Ventana::cambiar_escenario));  // conectar con ESCENARIO
  m_button7.signal_clicked().connect(SigC::slot(*this, &Ventana::obstaculo_movil));  // conectar con OBSTACULO_MOVIL
  m_button8.signal_clicked().connect(SigC::slot(*this, &Ventana::ctrl_reiniciar));  // conectar con CTRL_REINICIAR
  //m_button5.signal_clicked().connect(SigC::slot(*this, &Ventana::about));  // conectar con ABOUT    
  MyDialog* dialog = MyDialog::instance();
  m_button5.signal_clicked().connect(SigC::slot(*dialog, &Widget::show));


  m_box2.pack_start(m_box3, /*Gtk::PackOptions*/Gtk::PACK_EXPAND_WIDGET, /*padding*/5);
  m_box2.pack_start(m_button2, Gtk::PACK_SHRINK, 5);
  m_box2.pack_start(m_button3, Gtk::PACK_SHRINK, 5);
  m_box2.pack_start(m_button4, Gtk::PACK_SHRINK, 5);
  m_box2.pack_start(m_button6, Gtk::PACK_SHRINK, 5);
  m_box2.pack_start(m_button7, Gtk::PACK_SHRINK, 5);
  m_box2.pack_start(m_button8, Gtk::PACK_SHRINK, 5);


  m_box2.pack_start(m_box4, /*Gtk::PackOptions*/Gtk::PACK_EXPAND_WIDGET, /*padding*/5);
  m_box2.pack_start(m_button5, Gtk::PACK_SHRINK, 5);  // ACERCA DE
  m_box2.pack_start(m_button1, Gtk::PACK_SHRINK, 5);  // SALIR

  
  // box1
  m_area.set_size_request(400, 400);
  m_box1.pack_start(m_area, Gtk::PACK_EXPAND_WIDGET, 5);
  m_box1.pack_start(m_box2, Gtk::PACK_SHRINK, 5);
    
  // box0
  m_box0.pack_start(m_box1, Gtk::PACK_EXPAND_WIDGET, 5);
  
  set_border_width(10);
  add(m_box0);
  show_all();

  Glib::signal_timeout().connect(slot(*this,&Ventana::callback), 100);


  actuacion_izq=0;
  actuacion_dcha=0;
}

Ventana::~Ventana() {}

void Ventana::paso() {
  //tomar los valores de los sensores
  std::vector<unsigned> sensores = m_area.verObstaculos();
  cout << endl << " (robot) Lectura enviada: < " << sensores[0] << "  " << sensores[1] << "  " << sensores[2] << " > < " << sensores[3] << "  " << sensores[4] << " >    (sI sC sD) (dist ang)" << endl;

  // si ha encontrado el VERDE => dejar de ejecutar continuo; pasamos al paso a paso
  //if( sensores[0]>1000 ){
  //  ejecutar_continuo=false;
  //}

  //ENVIAR los valores de los sensores y ESPERAR recibir una actuación
  sincronizador *sincr = new sincronizador();
  sincr->enviar("S", sensores);
  std::vector<unsigned> actuacion = sincr->recibir("A");

  cout << " (robot) Actuacion recibida: [ "<< actuacion[0] << "  " << actuacion[1] <<" ]   (aI aD)" << endl;
  //informar al Area para que actualice el robot

  actuacion_izq = actuacion[0];
  actuacion_dcha= actuacion[1];

  //comprobar los códigos de CONTROL
  if( actuacion_izq<10000 ) {
    ejecutando_movimiento = true ;
    cout << " (robot) EJECUTAR MOVIMIENTO : I="<<actuacion_izq<<"  D="<<actuacion_dcha << endl;
    //increm. el contador de pasos dados hasta ahora
    m_area.incrementar_pasos();
  }else{
    // actuacion_izq >= 10000  =>  código de control
    switch( actuacion_dcha ) {
          case 0:  // parar
	    cout <<endl<<"\t\t (robot) DEBUG : parar el simulador (paso)"<<endl;
	    ejecutar_continuo=false;
	    break;
          case 1:  // reiniciar el simulador
	    cout <<endl<<"\t\t (robot) DEBUG : reiniciar el simulador"<<endl;
	    ctrl_reiniciar();
	    break;
          case 2:  // enviar datos: distancia,angulo,choques
	    cout <<endl<<"\t\t (robot) DEBUG : enviar datos al controlador  < dist ang choques pasos x y a xT yT >"<<endl;
	    ctrl_datos_posicion();
	    break;
          case 3:  // cod. ctrl OK  => no hace nada
	    cout <<endl<<"\t\t (robot) DEBUG : cod. ctrl.  OK"<<endl;
	    break;
    }
  }
}

void Ventana::ejecutar_movimiento() {
  cout << "\tCONTINUAR MOVIMIENTO : I="<<actuacion_izq<<"  D="<<actuacion_dcha << endl;
  if( actuacion_izq>0 && actuacion_dcha>0 ) {
    unsigned incr = (actuacion_izq<5) ? (actuacion_izq) : ( (actuacion_dcha<5)?actuacion_dcha: 5 ) ;
    m_area.avanzarRobot( incr, incr );
    actuacion_izq  -= incr;
    actuacion_dcha -= incr;
  }else{
    if( actuacion_izq>0 && actuacion_dcha<=0 ) {
      m_area.girarRobot( -1 , actuacion_izq );  //girar a la IZQ
      actuacion_izq=0;
    }
    if( actuacion_izq<=0 && actuacion_dcha>0 ) {
      m_area.girarRobot( 1 , actuacion_dcha );  //girar a la DCHA
      actuacion_dcha=0;
    }
    ejecutando_movimiento = false ;
    cout << " (robot) MOVIMIENTO EJECUTADO TOTALMENTE" << endl;
  }
}

bool Ventana::callback() {
  if( ejecutando_movimiento ) {
    ejecutar_movimiento();
  }else{
    if( ejecutar_continuo ) {
      //cout << "\n nuevo paso" << endl;
      paso();
    }
  }
  return (true); // TRUE->sigue  ; FALSE->para
}

void Ventana::pasoApaso() {
  ejecutar_continuo=false;
  paso();
}
void Ventana::continuo() {
  ejecutar_continuo=true;
}

void Ventana::mostrar_informacion() {
  m_area.mostrar_informacion();
}

void Ventana::cambiar_escenario() {
  m_area.cambiar_escenario();
}

void Ventana::obstaculo_movil() {
  m_area.obstaculo_movil();
}

void Ventana::ctrl_reiniciar() {
  //cout <<endl<<"\t\t DEBUG : reiniciar el simulador"<<endl;
  //ejecutar_continuo = false;
  m_area.ctrl_reiniciar();
}

void Ventana::ctrl_datos_posicion() {
  //tomar los valores del area
  // DEV.: < distancia angulo choques pasos x y a xT yT >
  std::vector<unsigned> sensores = m_area.ctrl_datos_posicion();
  cout << endl << " (robot) Lectura_DE_LOS_DATOS: < ";
  for(unsigned i=0 ; i < sensores.size() ; i++)  cout << " " << sensores[i] ;
  cout << " > " << endl;

  //ENVIAR los valores y ESPERAR recibir una actuación
  sincronizador *sincr = new sincronizador();
  sincr->enviar("S", sensores);
  std::vector<unsigned> actuacion = sincr->recibir("A");

  cout << " (robot) Actuacion_NO_USADA: [ "<< actuacion[0] << "  " << actuacion[1] <<" ]   (aI aD)" << endl;
}

//*****************************************************************************

int main(int argc, char** argv) {
  /*
    PARA LEER DEL FICHERO DE DEFINICION DE ESCENARIO...
    ifstream f;
    unsigned x,y;
    f.open( "k" );
    while( f ) {
      f >> x >> y ;
      if( !f.eof() ){
	cout << "x="<<x<<"  y="<<y << endl;
      }
    }
    f.close();
  */

   Gtk::Main main_instance (argc, argv);

   Ventana ventana;
   Gtk::Main::run( ventana );

   return 0;
}

